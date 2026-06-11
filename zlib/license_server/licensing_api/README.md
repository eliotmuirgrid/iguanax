Welcome to the licensing_api directory!
In here you will find a directory called mils_channels, in this directory is a git repo that can be used in an Iguana 6.0.0 and later.
To use this repo, just add it as a local repository in iguana settions on your iguana and import all the channels found in here.

VERY IMPORTANT: When exporting updated channels back into this repo from Iguana, make sure you run:
                                 hg add mils_channels/* 
before you commit and push in mercurial. Git may have created more object/pack files as a part of the export that will now need to be tracked by mercurial.

A little about the channels you'll find:
1. License Server API
	This is the main channel that powers MILS (my.interfaceware.com). 
2. Email Sender
	This channel is in charge of handling any emails that need to be sent to users
3. Health Check
	This channel is used to ensure that the iguana powering MILS is alive
4. Lua Unit Testing - MILS
	This channel contains a ton of unit tests that are run against files used in Licensing Server API
5. Encrypt CRM DB
	This channel is left running at all times. It will periodically check for any unencrypted passwords in the database and update them accordingly. Unencrpyted passwords could show up when a new individual is created directly in CRM or Pipedrive.

Other things you'll find in here:

licensing_config.lua and license_session_config.lua.
These 2 files must be placed in the working directory of the iguana you plan to use to run the channels above.
To find your working directory, simply open a translator in your iguana and use iguana.workingdir().

Some other important things:
- In ../licensing_ui/developement you will find another README file. This file contains important information about the compression of the UI files used to power MILS
- CRM and MILS use the same database (the databasae configs are found in licensing_config.lua mentioned above)
- memberslicense.exe
	Create this executable by running make in /memberslicense (a main level directory). This is also where the source code is found and where you'll be able to make changes.
	This file is used to generate the licenses required for MILS. This file must be placed in the working directory of the iguana running the Licensing Server API channel.
- ../nginx.conf. This file is the configuration file used on the server that runs the iguana that powers MILS. This is the configuration used by nginx on that server.