# AGENTS.md

Guidelines for AI coding agents working in this repository.

## Scope
- These instructions apply to the entire repository.
- If a deeper `AGENTS.md` exists in a subdirectory, the deeper file takes precedence for files in that subtree.

## Primary Goals
- Make minimal, correct changes that solve the user request end-to-end.
- Preserve existing behavior unless the task explicitly requires changes.
- Prefer clarity and maintainability over cleverness.

## Workflow
1. Read relevant files before editing.
2. Propose and implement the smallest viable change.
3. Run validation (tests/build/lint) relevant to changed code.
4. Summarize what changed and any remaining risks.

## Editing Rules
- Do not make unrelated refactors.
- Keep naming and style consistent with nearby code.
- Avoid adding new dependencies unless necessary.
- Update documentation when behavior or interfaces change.
- Never commit secrets, tokens, or private keys.

## Safety and Git
- Do not run destructive git/file operations unless explicitly requested.
- Do not revert or overwrite user changes outside your task scope.
- If unexpected repo changes appear mid-task, pause and report before proceeding.

## Testing and Verification
- Prefer targeted checks first, then broader checks if needed.
- If tests cannot be run, state that clearly and explain why.
- Include reproduction/verification steps for behavior changes.

## Communication
- Be concise and concrete.
- Call out assumptions and constraints.
- Provide file paths for all modified files.
- Report blockers immediately with the exact command/error.

## Definition of Done
- Requested behavior is implemented.
- Relevant validations pass (or failures are explained).
- Docs/config are updated if required.
- Summary includes what changed, why, and how it was verified.
