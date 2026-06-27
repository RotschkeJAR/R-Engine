# Error Handling

The engine provides utilities to track or identify the cause of an error.

## Console Output

The engine provides functions, that print information into the console, when something has to be noted for error detection.
It's recommended to use the macros instead of calling a function directly to print the exact location from where it has been printed into the console.

### Note

A state has been changed or data has been modified as a consequence of something else being done.
The action was implicit and expected behaviour.

### Warning

The software has entered a state, that is still valid, but later might cause an error, undefined behaviour, deadlock, corrupt or crash.
At the point in time, when the warning was raised, it cannot be determined, whether an error will to occur.

### Error

A specific task or function has failed and had to terminate its process.
The software will decide between using a different path to achieve the same goal without side effects or ignore it or raise a fatal error.

### Fatal Error

An operation has failed entirely and the software is obligated to terminate.
Restarting the engine is no longer allowed, the program has to be closed.

### Abort

The current state of the software is no longer certainly known or has been corrupted, that even termination can or is guranteed to fail.

## Signals

Calling the function `set_signal_handlers` can set function callbacks, that report the type of error, that caused the crash.
These functions are asynchronous and can interfere with console outputs from other concurrent threads.