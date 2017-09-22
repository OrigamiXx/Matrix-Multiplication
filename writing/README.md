# README

## Git tutorial

If you are not that familiar with git, check out [this
tutorial](https://bitbucket.org/dasarpmar/tutorial)

Follow the exercises listed there and you should get a good idea of
all the main commands.

## Using glatexdiff

I have also included a shell script `glatexdiff` with in this
directory. This is basically a small integration of git and a cool
program called `latexdiff`. First install the `latexdiff` program (on
my comp, it would be done via `sudo apt-get install latexdiff`). Once
you have that installed, you can basically use glatexdiff to create a
latexdiff of any two states in the repository. General usage is
`./glatexdiff [source state] [final state] [main file] >
diff.tex`. This would create a latexdiff on [main file] between the
two states, and dump the output in `diff.tex`.

````
#!console
$ ./glatexdiff master ramprasad main.tex  > diff.tex

$ ./glatexdiff f0b0576 6f2dae9 main.tex > diff.tex
````

Once you do this, you can compile diff.tex to see the changes.

`latexdiff` usually runs quite well but sometimes fails because the
diff might be within math equations or something. But in general this
is super useful.

**Note:** The .gitignore file is set up so that changes in diff.tex
  will not be tracked by git.