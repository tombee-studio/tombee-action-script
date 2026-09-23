# Tombee Action Script (TAS) 実装詳細設計書 (Implementation Design Document)

本書は、[SPEC.md](SPEC.md) で定義された Tombee Action Script (TAS) の言語仕様を C++ コードベースにおいて実現するための実装詳細設計書です。

---

## 1. 全体アーキテクチャ & モジュール構成

TAS のシステムは、コンパイルフロントエンド、中間表現/バイナリ層、仮想マシン実行エンジン、およびホスト連携ブリッジの4つの主要レイヤーで構成されます。

```mermaid
flowchart TD
    subgraph Frontend ["1. コンパイルフロントエンド"]
        LEX["Lexer (字句解析器)"]
        PARSER["Parser (構文解析器)"]
        AST["AST (抽象構文木)"]
        LEX --> PARSER --> AST
    end

    subgraph IR_Binary ["2. 中間表現 & バイナリ層"]
        TACGEN["TAC Generator (コード生成)"]
        TAC["TAC (3番地コード)"]
        COMPILER["BytecodeCompiler (シリアライザ)"]
        LOADER["BytecodeLoader (デシリアライザ)"]
        BIN[(".tasb バイナリデータ")]

        AST --> TACGEN --> TAC
        TAC --> COMPILER --> BIN
        BIN --> LOADER --> TAC
    end

    subgraph VM ["3. 仮想マシン実行エンジン (TAS VM)"]
        CPU["CPU / Stack VM"]
        SCOPE["ScopeManager (Global & Local)"]
        CORO["SequentialEngine (コルーチン/待機)"]
        EVENT_MGR["EventDispatcher (ディスパッチ/割り込み)"]

        CPU --> SCOPE
        CPU --> CORO
        CPU --> EVENT_MGR
        TAC --> CPU
    end

    subgraph Host ["4. ホスト連携レイヤー (tas_bridge)"]
        REG["TasHostRegistry (関数レジストリ)"]
        INST["TasScriptInstance (実行インスタンス)"]
        DOROTHY["Dorothy ゲームループ (C++)"]

        REG <--> CPU
        DOROTHY <--> INST
        INST --> CPU
    end
```

---

## 2. 字句解析器 (Lexer) & 構文解析器 (Parser) 設計

### 2.1 トークン定義の改修 (`token.hpp`)

`init`, `update`, `render` を予約語から除外し、一般的な識別子 (`TK_ID`) としてトークン化します。また、シーケンシャルコマンド記号 `@` をサポートします。

```cpp
namespace tas {

struct Token {
    enum TokenType {
        TK_EOF = -1,
        TK_INT,
        TK_FLOAT,
        TK_STRING,
        TK_ID,           // 変数名、カスタムイベント名、関数名（最大128文字）
        
        // 予約語 (Keywords)
        KW_FUNC,         // func
        KW_END,          // end
        KW_INTERRUPT,    // interrupt
        KW_IF,           // if
        KW_ELSE,         // else
        KW_LOOP,         // loop
        KW_INT,          // int
        KW_FLOAT,        // float
        KW_VAR,          // var
        KW_CASE,         // case (将来予約)
        KW_SHOT,         // shot (将来予約)
        KW_REF,          // ref (将来予約)
        
        // 演算子・記号
        OP_AT = '@',     // シーケンシャルコマンドプレフィックス
        OP_EQ = 256,     // ==
        OP_NE,           // !=
        OP_AND,          // &&
        OP_OR,           // ||
        OP_LE,           // <=
        OP_GE,           // >=
        OP_AA,           // +=
        OP_SA,           // -=
        OP_INC,          // ++
        OP_DEC,          // --
        COMMENT          // //
    };

    int type;
    union {
        int ival;
        double fval;
    };
    std::string id;      // 識別子または文字列リテラル

    // バリデーション定数
    static constexpr size_t MAX_IDENTIFIER_LENGTH = 128;
};

} // namespace tas
```

### 2.2 Lexer の改修ポイント (`lexer.cpp`)

1. **予約語テーブルの整理**:
   - `init`, `update`, `render` の判定コードを削除。
   - `tokenizeKeyword` の対象を `func`, `end`, `interrupt`, `if`, `else`, `loop`, `int`, `float`, `var`, `case`, `shot`, `ref` のみに限定。
2. **識別子の最大長チェック**:
   - `tokenizeId` でトークン抽出時、識別子長が 128 文字を超えている場合は `LexerError` を投げる。
3. **記号 `@` のトークン化**:
   - `tokenizeOperator(line, pos, '@')` をサポート。

---

### 2.3 AST クラス構造の再設計 (`ast.hpp`)

`SPEC.md` のハイブリッド構文（Global変数、シーケンシャルコマンド、カスタムイベント、割り込みイベント）を表現するために、AST クラス階層を再構築します。

```mermaid
classDiagram
    class Ast {
        <<abstract>>
        +print(tab)*
        +tac(operands, entries)*
    }

    class Statement {
        <<abstract>>
    }

    class Expression {
        <<abstract>>
        +ltac(operands, entries)*
    }

    class Script {
        -string _id
        -vector~Declare*~ _globals
        -vector~SequentialCommand*~ _seq_commands
        -map~string, CustomEvent*~ _events
        -vector~InterruptEvent*~ _interrupts
        +add_global(decl)
        +add_sequential(cmd)
        +add_event(event)
        +add_interrupt(intr)
    }

    class CustomEvent {
        -string _event_name
        -Statement* _body
        +tac(operands, entries)
    }

    class InterruptEvent {
        -Expression* _cond
        -Statement* _body
        +tac(operands, entries)
    }

    class SequentialCommand {
        -string _command_name
        -vector~Expression*~ _args
        +tac(operands, entries)
    }

    class Declare {
        -int _type
        -string _id
    }

    class Block {
        -vector~Ast*~ _statements
    }

    class IfSt {
        -Expression* _cond
        -Statement* _truest
        -Statement* _elsest
    }

    class Loop {
        -Expression* _times
        -Statement* _body
    }

    Ast <|-- Statement
    Ast <|-- Expression
    Ast <|-- Script
    Ast <|-- CustomEvent
    Ast <|-- InterruptEvent
    Ast <|-- SequentialCommand
    Statement <|-- Block
    Statement <|-- IfSt
    Statement <|-- Loop
    Expression <|-- Declare
```

---

### 2.4 Parser の構文解析ロジック (`parser.cpp`)

パーサは `func <id>:` から `end` までの間に現れる要素を先頭トークンで判別して分岐パースします。

```mermaid
flowchart TD
    START(["parse_script() 開始"]) --> MATCH_FUNC{"KW_FUNC かつ TK_ID ?"}
    MATCH_FUNC -- No --> ERR1["ParseError: expected 'func <id>:'"]
    MATCH_FUNC -- Yes --> PARSE_HEADER["Script オブジェクト生成<br>コロン ':' を消費"]

    PARSE_HEADER --> LOOP_START{"次のトークンは KW_END ?"}
    LOOP_START -- Yes --> DONE(["Script パース完了"])
    
    LOOP_START -- No --> BRANCH{"先頭トークンの判定"}
    
    BRANCH -->|KW_INT / KW_FLOAT / KW_VAR| P_GLOBAL["parse_declare()<br>-> Global変数登録"]
    BRANCH -->|OP_AT '@'| P_SEQ["parse_sequential_command()<br>-> シーケンシャルリスト登録"]
    BRANCH -->|KW_INTERRUPT| P_INTR["parse_interrupt()<br>-> 割り込みリスト登録"]
    BRANCH -->|TK_ID かつ 次が ':'| P_EV["parse_custom_event()<br>-> イベントマップ登録"]
    BRANCH -->|その他| P_ST["parse_statement()<br>-> 汎用文登録"]
    
    P_GLOBAL --> LOOP_START
    P_SEQ --> LOOP_START
    P_INTR --> LOOP_START
    P_EV --> LOOP_START
    P_ST --> LOOP_START
```

---

## 3. 中間表現 (TAC) & バイナリフォーマット設計

### 3.1 TAC 命令セットの拡張 (`tac.hpp`)

コルーチン待機 (`DELAY`)、イベント発行 (`DISPATCH`)、Global変数宣言 (`GLOBAL_DECL`) を新規オペコードとして定義します。

```cpp
namespace tas {

struct TACOperand {
    enum OpCode {
        EXIT = -1,
        NEXT = 0,
        PUSH = 1,
        POP = 2,
        DECL = 3,          // ローカル変数宣言
        ASSIGN = 4,
        EQ = 5,
        NE = 6,
        LT = 7,
        LE = 8,
        GT = 9,
        GE = 10,
        ADD = 11,
        SUB = 12,
        MUL = 13,
        DIV = 14,
        MOD = 15,
        REV = 16,
        LOAD = 17,
        CALL = 18,
        JE = 19,
        JNE = 20,
        JMP = 21,
        STAGED = 22,       // ローカルスコープ開始
        UNSTAGED = 23,     // ローカルスコープ終了
        LOOPSTART = 24,
        LOOPEND = 25,
        
        // 拡張オペコード
        GLOBAL_DECL = 26,  // スクリプトGlobal変数の宣言
        DELAY = 27,        // シーケンシャル待機 (引数: ticks/ms)
        DISPATCH = 28,     // イベント発行 (引数: event_name)
        YIELD = 29         // 1ステップ実行譲渡
    };

    int mnemonic;
    int type;
    Primitive value;
    int argsNum;

    static TACOperand make(int mnemonic, int type, Primitive val, int argsNum = 0);
};

} // namespace tas
```

---

### 3.2 `.tasb` (TAS Binary) ファイルフォーマット設計

事前コンパイルされたバイナリファイルのレイアウト仕様です。

```mermaid
packet-beta
0-31: "Magic: 'TASB' (0x54415342)"
32-47: "Format Version (0x0100)"
48-63: "Flags / Padding"
64-95: "Global Variable Area Size (uint32)"
96-127: "Sequential Entry PC (int32, -1 if none)"
128-159: "String Pool Offset / Count"
160-191: "Event Symbol Table Offset / Count"
192-223: "Interrupt Table Offset / Count"
224-255: "Code Section Offset / Instruction Count"
```

#### セクション構造一覧

| セクション | フィールド | 型 | 説明 |
| :--- | :--- | :--- | :--- |
| **Header** | `magic` | `uint32_t` | `'T','A','S','B'` (`0x54415342`) |
| | `version` | `uint16_t` | バイナリ仕様バージョン (例: `1`) |
| | `flags` | `uint16_t` | デバッグ情報有無等のフラグ |
| | `global_count` | `uint32_t` | Global変数の予約領域サイズ |
| | `seq_entry_pc` | `int32_t` | シーケンシャル構文の開始PC (無ければ `-1`) |
| **String Pool** | `string_count` | `uint32_t` | 文字列テーブルの要素数 |
| | `entries[]` | `StringEntry` | 文字列長 (`uint16_t`) + ASCIIバイト列 |
| **Event Table** | `event_count` | `uint32_t` | カスタムイベント数 |
| | `events[]` | `EventEntry` | `name_str_index (uint32_t)`, `entry_pc (uint32_t)` |
| **Interrupt Table**| `intr_count` | `uint32_t` | 割り込み定義数 |
| | `interrupts[]`| `IntrEntry` | `cond_pc (uint32_t)`, `handler_pc (uint32_t)` |
| **Code Section** | `code_count` | `uint32_t` | TAC 命令数 |
| | `instructions[]`| `TACBinary` | `mnemonic (uint8)`, `type (uint8)`, `args (uint16)`, `val` |

#### `BytecodeCompiler` & `BytecodeLoader`
- `BytecodeCompiler::compile(const Program& prog, std::ostream& out)`: AST/TAC から `.tasb` バイナリを出力。
- `BytecodeLoader::load(std::istream& in, Program& out)`: バイナリを読み込み、VM が直接実行可能な構造を高速構築。

---

## 4. 仮想マシン (TAS VM / CPU) 実行エンジン設計

### 4.1 メモリ＆スコープ管理 (`ScopeManager`)

スクリプトスコープの **Global変数** とブロックスコープの **Local変数** を明確に分離管理します。

```mermaid
classDiagram
    class CPU {
        -map~string, Primitive~ _global_table
        -vector~map~string, Primitive~~ _local_tables
        -Primitive _stack[512]
        -int _sp
        -int _pc
        -vector~TACOperand~ _codes
        -map~string, int~ _event_entries
        -int _seq_pc
        -int _delay_ticks
        -ExecutionState _state
        +start_event(event_name)
        +step_sequential()
        +declare_global(name, val)
        +declare_local(name, val)
        +find_variable(name) Primitive*
    }
```

#### 変数解決（ルックアップ）アルゴリズム
1. 現在のローカルスコープスタックの最上位 (`_local_tables.back()`) から順に親スコープへと探索。
2. ローカルスタック内に存在しない場合、スクリプトの `_global_table` を探索。
3. いずれにも存在しない場合は `nullptr` を返し、代入時は `_global_table` またはカレントローカルに新規確保。

---

### 4.2 シーケンシャル実行 & サスペンド制御 (`SequentialEngine`)

シーケンシャルコマンドはコルーチンのようにフレームを跨いで実行されます。

```mermaid
stateDiagram-v2
    [*] --> Idle : スクリプトロード
    Idle --> Running : start_sequential()
    
    state Running {
        [*] --> ExecuteCommand
        ExecuteCommand --> DelayEncountered : TACOperand::DELAY(ticks)
        DelayEncountered --> Suspended : _delay_ticks = ticks
        
        ExecuteCommand --> DispatchEncountered : TACOperand::DISPATCH(name)
        DispatchEncountered --> TriggerEvent : イベントディスパッチ
        TriggerEvent --> ExecuteCommand
    }
    
    Suspended --> Suspended : step_sequential() [ticks > 0 / ticks--]
    Suspended --> Running : step_sequential() [ticks == 0]
    
    Running --> Finished : TACOperand::EXIT
    Finished --> [*]
```

#### `step_sequential()` の処理フロー
1. `_state == Suspended` の場合:
   - `_delay_ticks > 0` ならデクリメントして処理を戻す（次回フレームへ譲渡）。
   - `_delay_ticks == 0` になったら `_state = Running` に遷移。
2. `_state == Running` の場合:
   - `_seq_pc` の TAC 命令を1命令ずつ実行。
   - `DELAY` 命令に到達したら `_delay_ticks` をセットして `Suspended` に遷移し中断。
   - `DISPATCH` 命令に到達したら指定イベントをディスパッチ。
   - `EXIT` 命令に到達したら `_state = Finished` に遷移。

---

### 4.3 イベントディスパッチ & 割り込み監視

#### 1. カスタムイベント呼び出し (`dispatch`)
- `dispatch(string event_name)` が呼ばれると、`_event_entries` から該当イベントの開始 PC を取得。
- イベントハンドラ実行用に独立したローカルスコープ `_local_tables` を作成し、`EXIT` に達するまで実行。

#### 2. 条件付き割り込み監視 (`interrupt`)
- 毎フレームの更新時（またはシーケンシャルステップ前）に登録されているすべての割り込み条件式 (`cond_pc`) を一時的に評価。
- 条件式の結果が真 (`true` / 非0) と評価された場合、対応する `handler_pc` を即座に実行。

---

## 5. ホスト連携設計 (`tas_bridge`)

C++ / Dorothy ゲームエンジンと TAS スクリプトインスタンスの接続インターフェースです。

```mermaid
sequenceDiagram
    autonumber
    participant Engine as Dorothy ゲームループ
    participant Bridge as TasHostRegistry / Bridge
    participant Instance as TasScriptInstance
    participant VM as TAS CPU

    Engine->>Bridge: ホスト関数登録 (set_speed, spawn_bullet等)
    Engine->>Instance: スクリプト生成 ("enemy_boss.tas" / ".tasb")
    Instance->>VM: ロード & 初期化
    
    Engine->>Instance: dispatch_event("init")
    Instance->>VM: init: { ... } を実行
    
    loop 毎フレーム update(dt)
        Engine->>Instance: update(dt)
        Instance->>VM: check_interrupts() (割り込み条件監視)
        Instance->>VM: step_sequential() (待機タイマー減算 & 次コマンド)
        Instance->>VM: dispatch_event("update")
    end
```

### クラス設計 (`tas_bridge.hpp`)

```cpp
namespace tas_bridge {

class TasScriptInstance {
private:
    tas::CPU vm_;
    std::string script_id_;
    bool is_alive_;

public:
    TasScriptInstance(const std::string& script_name);
    
    // ソースまたはバイナリのロード
    bool load_source(const std::string& source_code);
    bool load_binary(const std::string& binary_file);
    
    // ゲームループ更新
    void update(float dt);
    
    // イベント発行
    void dispatch(const std::string& event_name);
    
    // 状態取得
    bool is_finished() const;
    void set_context_data(void* data);
};

} // namespace tas_bridge
```

---

## 6. 実装ステップ & ロードマップ

```mermaid
gantt
    title TAS 実装ロードマップ
    dateFormat  YYYY-MM-DD
    section Phase 1: 字句・構文解析
    予約語整理 & 128文字制限バリデーション :p1_1, 2026-09-25, 2d
    AST クラス階層再設計 (CustomEvent, SeqCmd) :p1_2, after p1_1, 3d
    Parser 拡張 (Global変数, @コマンド, イベント) :p1_3, after p1_2, 3d

    section Phase 2: TAC & バイナリ
    TAC 命令拡張 (DELAY, DISPATCH, GLOBAL_DECL) :p2_1, after p1_3, 2d
    BytecodeCompiler / Loader 実装 (.tasb) :p2_2, after p2_1, 4d

    section Phase 3: VM 実行エンジン
    Global / Local スコープ分離実装 :p3_1, after p2_2, 3d
    SequentialEngine (サスペンド/リジューム) :p3_2, after p3_1, 4d
    EventDispatcher & 割り込み監視実装 :p3_3, after p3_2, 3d

    section Phase 4: 結合 & テスト
    tas_bridge 連携 & Dorothy 統合 :p4_1, after p3_3, 3d
    単体テスト・結合テストの作成 :p4_2, after p4_1, 4d
```

---

## 7. 結論と品質検証方針

本詳細設計書に基づき実装を行うことで、以下の品質基準を達成します。

1. **言語仕様準拠性**: [SPEC.md](SPEC.md) に定義されたハイブリッド構文（イベントドリブン＋シーケンシャル）、スコープ規則、ローワースネークケース命名規則を完全充足。
2. **パフォーマンス**: 事前コンパイルバイナリ（`.tasb`）のロード実行により、起動時間およびメモリオーバーヘッドを極小化。
3. **拡張性 & 安全性**: 識別子長 128 文字制限、ゼロ除算保護、スタックオーバーフロー保護による高い耐障害性を担保。
