# mini-shell
This is a project for the university to create a simple version of a shell.

## Running
- To compile the code type:
`make`

- To run it:
`make run`

- To compile the test type:
`make test`

- To run all tests:
`make test_run`

- A message will show:
```{bash}
Welcome to the miniature-shell
```

## Examples of commands:
```{bash}
./scripts/prog_with_args.sh 12 100
/bin/ls
/bin/cat ins/in.txt
/bin/cat ins/in.txt > outs/out.txt
./scripts/prog.sh < ins/num_in.txt
./scripts/prog.sh < ins/num_in.txt > outs/num_out.txt

scripts/prog_with_args.sh 12 100
ls
cat ins/in.txt
cat ins/in.txt > outs/out.txt
scripts/prog.sh < ins/num_in.txt
scripts/prog.sh < ins/num_in.txt > outs/num_out.txt
ls | sort | grep ins
```
