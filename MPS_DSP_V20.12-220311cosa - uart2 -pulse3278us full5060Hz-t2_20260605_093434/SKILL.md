---
name: software-revision
description: >
  Use this skill whenever the user wants to systematically analyze, refactor, optimize, or document a software codebase — especially embedded C projects (dsPIC, ARM, DSP, MCU firmware). Triggers include: "analyze this codebase", "refactor this project", "optimize the system", "add comprehensive comments", "improve code quality", "fix encoding issues", "create a revision report", "clean up code", "make this production-ready", "modernize this firmware", or any request to apply a structured improvement process to a code project. Also trigger when the user references "MPS workflow", "software revision methodology", or asks for a multi-stage code improvement plan.
  The skill provides a proven 6-stage methodology covering: (1) project understanding and encoding fixes, (2) safety/maintainability/portability refactoring, (3) comprehensive annotation, (4) workflow optimization, (5) architectural optimization, and (6) documentation output. Each stage saves results to versioned folders with detailed change reports.
---

# Software System Revision Skill

This skill implements a systematic 6-stage methodology for analyzing, revising, and optimizing software codebases — proven on industrial embedded C firmware (dsPIC motor controllers) but applicable to any C/C++ project.

## Core Principles

- **Work incrementally**: Each stage builds on the previous, saved to versioned folders (e.g., `_refactored/`, `_V20.03_optimized/`, `_V20.04_optimized/`)
- **Preserve behavior**: All changes must maintain functional equivalence unless explicitly improving safety (e.g., enabling watchdog)
- **Document everything**: Every stage outputs a detailed markdown report explaining what changed, why, and the before/after comparison
- **Backward compatible**: Communication protocols, EEPROM layouts, and external interfaces must remain compatible
- **ASCII-only for diagrams**: All markdown documents containing ASCII art flowcharts, timing diagrams, or box-drawing layouts MUST use only ASCII characters (English labels, `├┤│─└┌┐┘` box-drawing). Chinese text in the diagram body causes column misalignment because Chinese characters are 2x width while box-drawing characters are 1x width. Place Chinese annotations as a separate legend block *below* the diagram, not inside it.

## The 6-Stage Methodology

### Stage 1: Project Understanding & Encoding Fix

**Goal**: Understand the entire codebase and make it readable.

1. **Read all source files**: .c, .h, .s (assembly), Makefile, linker scripts, configuration files
2. **Detect and fix encoding issues**:
   - Check if comments are garbled → detect encoding (try gb2312/gbk for Chinese projects)
   - Convert all files to UTF-8 using Python: `data.decode('gbk').encode('utf-8')`
3. **Generate a comprehensive analysis report** containing:
   - Project overview (MCU, compiler, version, copyright)
   - File structure map with each file's purpose
   - Hardware resource allocation (ADC channels, timers, UARTs, GPIOs)
   - Software module breakdown (one subsection per source file)
   - System workflow (startup sequence, main loop, interrupt structure)
   - Pros/cons analysis (safety, maintainability, portability, performance)
   - Optimization recommendations (prioritized as high/medium/low)
   - Version history and key technical parameters

**Output**: `系统分析报告.md` (System Analysis Report) in the project root.

### Stage 2: Code Refactoring (Safety + Maintainability + Portability)

**Goal**: Fix safety issues, reduce coupling, eliminate duplication, improve portability.

**Safety fixes** (always do first):
- Enable watchdog if disabled: `_FWDT(WDT_OFF)` → `_FWDT(WDT_EN)`
- Protect EEPROM/Flash writes: add DISI instruction + save/restore interrupt state + raise IPL
- Extract common write-protect logic into `EnterWriteProtect()`/`ExitWriteProtect()` helpers

**Maintainability fixes**:
- Convert bare integer state variables to enums (e.g., `step11` → `eStepState`)
- Extract duplicated code blocks into helper functions (aim for >100 lines of duplication eliminated)
- Add `static` to all file-scope variables not referenced via `extern` (scan headers and source files to identify truly-global vs file-local variables)
- Replace magic numbers with named #define constants where feasible

**Portability fixes**:
- Rewrite Makefile to be cross-platform: use variables for tool paths, auto-detect OS, separate build/output directories
- Remove hardcoded Windows paths, `.exe` suffixes, and backslash path separators

**Output folder**: `_refactored/`
**Output document**: `修订说明.md` with before/after code snippets and rationale for each change.

### Stage 3: Comprehensive Annotation

**Goal**: Every function and critical code block has clear explanatory comments.

1. **Function header comments**: Purpose, parameters, return value, algorithm notes, calling context
2. **Key statement comments**: Explain WHY not WHAT — focus on non-obvious logic, hardware constraints, algorithm rationale
3. **Data structure comments**: Each struct field annotated with meaning and units
4. **Module header comments**: File purpose, MCU target, compiler, revision history
5. **Comment language**: Match the project's existing comment language (Chinese projects → Chinese comments)

**Output**: All source files overwritten with annotated versions in the same folder.

### Stage 4: Workflow Optimization

**Goal**: Improve runtime behavior without architectural changes.

1. **Task scheduling layering**: Split monolithic 10ms loop into 1ms/10ms/100ms tiers based on real-time requirements
   - 1ms: fast protection (in ISR)
   - 10ms: core control (ADC scaling, protection, state machine, soft start)
   - 100ms: slow tasks (diagnostics, statistics, frequency detection if LT not affected, IO scan)
2. **Protection fast channel**: Add immediate fault detection for extreme conditions (e.g., 6× rated overcurrent → 1ms response via ISR)
3. **Logic simplification**: Replace multi-path (>100 line) if-else chains with unified condition functions
4. **Event-driven communication**: Only call UART processing when data flags are set, not every 10ms
5. **Runtime logging**: Use unused EEPROM/Flash space for circular logs (last N starts, last M faults)

**Output folder**: `_V20.03_optimized/` (or next version number)
**Output document**: `优化说明.md` with optimization rationale, before/after comparison table, and compatibility notes.

### Stage 5: Architectural Optimization

**Goal**: Restructure code for better maintainability and add advanced features.

1. **State machine table-driven refactoring**: Replace large if-else chains with state handler arrays + transition tables
2. **Lookup table simplification**: Replace repetitive switch/if-else with data-driven lookup tables (e.g., trigger pulse sequences)
3. **Closed-loop control upgrade**: Add PI/PID controller option alongside fixed-step algorithm, switchable via EEPROM flag
4. **Parameter integrity**: Add CRC16 verification of configuration parameters with automatic backup/restore
5. **Self-diagnosis system**: Add online health checks for sensors (CT/PT break), actuators (SCR aging, contactor sticking), and references (ADC voltage)

**Output folder**: `_V20.04_optimized/`
**Output document**: `修订说明及结论.md` with cumulative improvement summary across all versions.

### Stage 6: Documentation Output

**Goal**: Produce comprehensive comparison and flow documentation.

Always produce these three documents when the user asks for detailed flow analysis:

1. **Startup/stop flow comparison**: Original vs revised TurnRunF detection, state transition diagrams
2. **Control algorithm flow**: Detailed flow for constant-current/PID/soft-start control loops
3. **Complete system flow**: All subsystems (initialization, scheduling, interrupts, state machine, protection, communication, data acquisition, EEPROM layout)

Each document should include:
- ASCII art flowcharts showing before/after (pure ASCII English labels inside diagram; Chinese legend below)
- Code snippet comparisons
- Timing diagrams where relevant (pure ASCII English labels; Chinese annotations below)
- Parameter reference tables

## Output Conventions

### Folder Naming
```
ProjectName_V<major>.<minor>_<stage>/
├── src/           # Source files
├── h/             # Headers
├── gld/           # Linker scripts (if applicable)
├── Makefile       # Build system
├── *.md           # Documentation
```

### Document Naming
- `系统分析报告.md` — Stage 1 system analysis
- `修订说明.md` — Stage 2 refactoring report
- `优化说明_V<version>.md` — Stage 4 workflow optimization report
- `V<version>_修订说明及结论.md` — Stage 5 architectural optimization report
- `<topic>流程详解.md` — Stage 6 flow documentation
- `调度架构修订对比详解.md` — Scheduling architecture comparison
- `后续优化建议.md` — Future optimization roadmap

### Document Structure Pattern
Each revision document should follow this structure:
```markdown
# Title

## 一、概述 (Overview)
## 二、各优化详细说明 (Detailed per-change explanation)
  - 问题原因 (Problem)
  - 修改位置 (File location)
  - 修改前 (Before code)
  - 修改后 (After code)
  - 效果 (Effect)
## 三、修改文件清单 (Changed files)
## 四、功能对比总表 (Feature comparison table)
## 五、兼容性说明 (Compatibility)
## 六、验证建议 (Verification suggestions)
```


## ASCII Diagram Formatting Rule (MANDATORY)

When creating any markdown document containing ASCII art diagrams:

**ALWAYS use pure ASCII English inside the diagram code block.** Chinese characters occupy 2 display columns while ASCII box-drawing chars (`├┤│─└┌┐┘`) occupy 1 column — mixing them breaks column alignment.

**Place Chinese annotations as legend BELOW the diagram:**
```
> Legend: confirm=确认, dead=死区, Z-cross=过零, trigger delay=触发角延时
```

## Working with Embedded C Specifics

When working with MCU firmware:

- **Never remove hardware initialization sequences** unless clearly redundant
- **Preserve interrupt priority assignments** — they are tuned for the specific application
- **EEPROM/Flash writes must be atomic** — always add protection before modifying write functions
- **Bit-field structs** (e.g., `unsigned field:1`) are compiler-dependent — note this in comments but don't change the layout
- **Compiler builtins** (e.g., `__builtin_mulss`, `__attribute__((interrupt))`) are platform-specific — document them, don't replace them
- **ASM blocks** (e.g., `asm("CLRWDT")`) must be preserved exactly
- **Fuse configuration bits** (e.g., `_FWDT()`, `_FOSC()`) must match the hardware design

## Quick Reference: Common Fix Patterns

```c
// 1. Watchdog enable
// Before: _FWDT(WDT_OFF);
// After:  _FWDT(WDT_EN);

// 2. EEPROM write protection
// Before: SET_CPU_IPL(7); ... write ... SET_CPU_IPL(3);
// After:  save_ipl/iec0 + __builtin_disi(0x3FFF) + SET_CPU_IPL(7) + write + restore

// 3. State machine enum
// Before: unsigned int step11 = 0;
// After:  eStepState step11 = STEP_WAIT;

// 4. Duplicated code → helper function
// Before: 100+ lines repeated in 2+ functions
// After:  static unsigned int PackInputBits(void) { ... }

// 5. Global → static
// Before: unsigned int delayCnt;
// After:  static unsigned int delayCnt;
```

## References

See `references/mps_example.md` for a complete walkthrough of this methodology applied to the MPS high-voltage soft starter DSP project, including actual code examples and output documents.
