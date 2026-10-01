# Contributing to Atomica

Everyone commits under **their own GitHub account**, regularly, in small commits. Do not push everything in bulk at the end: commit history is used as evidence of individual contribution.

## One-time setup

```bash
git clone <REPO_URL>
cd atomica
git config user.name  "Your Name"
git config user.email "the-email-linked-to-your-github-account"
```

## Your branch

Create your own branch from `main` and work only there:

```bash
git checkout main
git pull
git checkout -b <your-name>/<stage-or-task>     # e.g. gloria/stage3-waves
git push -u origin <your-name>/<stage-or-task>
```

## Daily workflow

```bash
git pull origin main           # pick up teammates' merged work
# ...edit code...
git add <files>
git commit -m "stage3: draw Bezier standing wave for n=2"
git push
```

## Merging into main

1. Push your branch.
2. Open a **Pull Request** into `main` on GitHub.
3. At least one teammate reviews it (the integration owner does a final check).
4. Merge once it builds and runs.

## Rules

- Never commit directly to `main`.
- Keep your code inside your stage's folder unless you are changing shared code in `src/core/`; tell the group first if you are.
- Commit messages start with the stage or area: `stage1:`, `stage2:`, ... `ui:`, `core:`, `docs:`.
- Don't commit build output (`build/`, `.exe`, `.o`) or IDE folders.
- If you hit a merge conflict, ask for help rather than overwriting someone else's work.
