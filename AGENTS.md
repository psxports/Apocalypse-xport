# Apocalypse (xport)

Resolve `[XPORT_ROOT]` from `xport-project.json` and read `[XPORT_ROOT]/AGENTS.md`. Keep only stable game-specific facts and task-routing links here; put detailed evidence under `status`.

## Project facts

- Native short name: `A`; languages: C/C++, migrating game code to C++98
- Solution: `src/platform/win/A.sln`; executable: `bin/A.exe`; working directory: `bin`; intermediates: `_build`
- Runtime data: `bin/DATA`; Red Book output when applicable: `bin/MUSIC`
- Before relying on them, record reviewed image identities, language decision, dummy scope, hooks/layouts, adapter contract and evidence links here

- Reviewed original image: `SLUS_003.73`, SHA-256 `9975e88250fecddb9828cd8bcbc87cb901018df0a103eb7387890bf2858e67e8`
- Runtime GP: `0x800FEEBC`, proved by the adjacent initializer at `0x80086218`; evidence: [status/audits/coverage-first/gp-provenance.json](status/audits/coverage-first/gp-provenance.json)
- Corrected-GP IDA export: `status/ida/images/coverage-first-gp-v1`; acceptance receipt: [status/ida/accepted/coverage-first-gp-v1.json](status/ida/accepted/coverage-first-gp-v1.json). Keep the old `orig/images` export immutable; coverage packets must bind the selected export explicitly before using revised pseudocode
- C++ module migration plan: [status/cpp-migration/plan.json](status/cpp-migration/plan.json); preserve current memory/callback contracts and historical source-bound evidence during the language transition
- Current implementation workflow: [status/cpp-migration/implementation-first.json](status/cpp-migration/implementation-first.json); human requested all remaining functions and modules before any builds or checks, including standalone fixtures and source-index/style validation

- Game C++ dialect: C++98 per human instruction; use classic virtual declarations, unscoped enums, `0` for null pointers and explicit destructor bodies. Do not introduce C++11 or later features. Preserve SDK/ABI type and arithmetic requirements
- Current module organization and naming: [status/module-organization/plan.json](status/module-organization/plan.json); original-address owner map: [status/module-organization/module-map.json](status/module-organization/module-map.json); preserve the retained ABI identities and role-based naming qualifications
