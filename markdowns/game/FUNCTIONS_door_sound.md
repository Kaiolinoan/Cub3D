# door_sound.c — Function Breakdown

## Function: `init_door_audio(void)`

```c
void	init_door_audio(void)
{
	signal(SIGCHLD, SIG_IGN);
}
```

**Purpose**: Initialize audio system by ignoring child process signals.

**Parameters:** None.

**Return Value:** None (void).

**Logic:**

- Register signal handler for `SIGCHLD` (child process termination).
- Set to `SIG_IGN` to ignore child exit signals.
- Prevents zombie processes from child audio player processes.

**Why Needed:**
- `play_sound()` forks child processes to play audio.
- Without this, terminated child processes become zombies until parent reaps them.
- Setting to `SIG_IGN` automatically reaps children.

---

## Function: `silence_std_fds(void)` (static)

```c
static void	silence_std_fds(void)
{
	int	fd;

	fd = open("/dev/null", O_WRONLY);
	if (fd < 0)
		return ;
	dup2(fd, STDOUT_FILENO);
	dup2(fd, STDERR_FILENO);
	close(fd);
}
```

**Purpose**: Redirect stdout and stderr to /dev/null to suppress audio player output.

**Parameters:** None.

**Return Value:** None (void).

**Logic:**

1. **Open /dev/null** — Open null device in write mode.

2. **Redirect stdout** — `dup2(fd, STDOUT_FILENO)` redirects fd 1 to /dev/null.

3. **Redirect stderr** — `dup2(fd, STDERR_FILENO)` redirects fd 2 to /dev/null.

4. **Close file** — Close the /dev/null file descriptor (stdout/stderr copies remain open).

**Effect:**
- Audio player (`paplay`) output and errors are suppressed.
- Prevents cluttering the terminal with audio messages.

---

## Function: `play_sound(char *path)`

```c
void	play_sound(char *path)
{
	pid_t	pid;

	pid = fork();
	if (pid < 0)
		return ;
	if (pid == 0)
	{
		silence_std_fds();
		execlp("paplay", "paplay", path, NULL);
		_exit(1);
	}
}
```

**Purpose**: Play an audio file asynchronously via `paplay` (PulseAudio command-line player).

**Parameters:**
- `path`: Path to audio file (e.g., `"assets/sounds/door_open.wav"`).

**Return Value:** None (void).

**Logic:**

1. **Fork process** — Create child process:
   - Parent: continues immediately (non-blocking).
   - Child: executes audio playback.

2. **Fork error** — If fork fails, return silently.

3. **Child process**:
   - Call `silence_std_fds()` to suppress output.
   - Call `execlp()` to execute `paplay <path>`:
     - `execlp` replaces process image, no return if successful.
     - Argument list: `"paplay", path, NULL`.
   - If `execlp` fails, call `_exit(1)` to exit child (shouldn't reach if paplay exists).

**Execution:**
- Parent process returns immediately; audio plays in background child.
- Child process exits when audio finishes.
- Parent has registered `SIGCHLD` handler to reap child.

**Audio Files:**
- Typically in `assets/sounds/` directory.
- Defined as constants: `DOOR_OPEN_SOUND`, `DOOR_CLOSE_SOUND`.

**Requirements:**
- System must have `paplay` command (part of PulseAudio).
- Audio files must exist at specified paths.
