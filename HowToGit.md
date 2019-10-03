# How to set up and use Git

## Setup

* In the command line, move to the directory where you want to have a working copy of the eclipse workspace on the repository, most likely in your eclipse workspace directory.
* Before copying a working copy, we first should set up various git settings:

	```
	git config --global user.name "name" (substitute name, leave "")
	git config --global user.email "mail" (same with mail)

	git config --global color.ui true
	git config --global color.status auto
	git config --global color.branch auto
	git config --global merge.tool vimdiff
	```

substitute name and mail for your working name and mail
* type ``` git clone https://git.cs.bham.ac.uk/robotics-group4-o4/assignment1```, it will now copy the repository from the gitlab server to your local directory.
* type ``` git remote add origin https://git.cs.bham.ac.uk/robotics-group4-o4/assignment1```, this sets a shortcut for the url to the variable 'origin'.

## Using GIT

#### Add and commit

* After the setup you are handling your cloned working directory just like any other local file directory and open and use it in eclipse.
* As soon as you change files and save these changes and want to save the state of your current git directory as a commit, you can save the state of your files as following:
* First ``` git add fileOrFolderName``` (pass git add multiple files or use it as many times as you want)
* Following, use ``` git commit -m "Message documenting the changes"```
* Now your working state is saved as a commit, you can return to this state at any time, how exactly is not important for now. Of course saving the files in eclipse will save the files just as before, but will not automatically update the git saves (commits)
* After committing your state the directory commit is only saved locally, the repository at the server-side is not updated yet.

#### Push

* To update the git repository so everybody can access the new version, you need to type ``` git push origin master```. Depending on privacy settings of the account the information is pushed to, you might need to log into the page in your terminal window. Make sure to log into the browser version of GitLab at ```https://git.cs.bham.ac.uk``` at least once before trying to push to the repository since it might not authenticate you otherwise.
* origin is referring to the URL that your local directory will be saved to now, master is the default branch, more on that follow - for now you should ensure the code you want to push works and then upload via ```git push origin master``` to the master branch.

#### Status

* to see the state your git is currently in, use ``` git status``` this will print all tracked, changed (also added) files and show untracked changes that aren't added to the commit yet.

#### Branches

* type ```git branch``` to get a status on which branch you are on and which are available.
* default branch is ```master```.
* type ```git branch branchName``` to create a branch.
* type ```git branch -d branchName``` to delete a branch.
* for now branching limited to local repository copy, useable on server-side but more complex.
* use ```git checkout branchName``` to change into this branch.
* use ```git merge branchName``` to merge branches. branchName will be the branch you want to get the updated version from. E.g. you make changes to the files in branch dev and want the master branch to be "up to date". You will use ```git checkout master``` to move to the master branch and call ```git merge dev``` to get master updated with the current content of dev. If you merge two branches that made changes to e.g. the same file, you will get a merge message that you need to solve the merge differences.
