# Math Packet Client and Server

## Make targets

### Valgrind
You can run valgrind on the client or server by running the following commands in the terminal:

```
make valgrind_client
```

or

```
make valgrind_server
```

If you need to change the command line options sent to your [client](./Makefile#L29) or [server](./Makefile#L32) with valgrind, check out the Makefile.


### Printing to PDF

To print the source files to a PDF you can run these make commands at the terminal:

```
make printClient
make printServer
make printErrors
```

Each of these commands will produce a PDF file in the bin directory of your project.

## Debugging!

There are two debug profiles specified in [launch.json](./.vscode/launch.json).

_(gdb) Launch Client with args_ will debug the client with the command line args given in the args parameter in launch.json [line 12](./.vscode/launch.json#L12).

_(gdb) Launch Server with args_ will debug the client with the command line args given in the args parameter in launch.json [line 32](./.vscode/launch.json#L32).

You can edit these two lines in VS Code if you need to debug your code with different command line options.
