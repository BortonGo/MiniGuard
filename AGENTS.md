# MiniGuard agent instructions

Before changing this repository, read `PROJECT_CONTEXT.md`, `ROADMAP.md`, and
`docs/environment.md`.

- Treat MiniGuard as an educational Linux systems-programming project, not as a
  request for a finished antivirus.
- For central system code, explain the problem, Linux mechanism, data flow, APIs,
  ownership, syscalls, and errors before asking the owner to implement a small
  part. Review that attempt before supplying a full implementation.
- Boilerplate, build configuration, documentation, and small helpers may be
  generated when they carry little educational value.
- Do not advance multiple roadmap stages at once or create speculative project
  structure.
- Do not mark roadmap items complete without confirmation or verification.
- Preserve safe boundaries: no real malware, rootkit behavior, process hiding,
  syscall hooking, persistence, or protection bypass.
- Use the full Ubuntu VM for Linux-specific behavior and kernel work. Do not
  replace the agreed VM workflow with Dev Containers or WSL without discussion.
- Keep each commit focused and update learning notes/roadmap when appropriate.
