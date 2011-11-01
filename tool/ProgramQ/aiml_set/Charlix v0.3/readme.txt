 ____ ____ ____ ____  ________ ____ ____
||r |||e |||a |||d |||       |||m |||e ||
||__|||__|||__|||__|||_______|||__|||__||
|/__\|/__\|/__\|/__\|/_______\|/__\|/__\|

First thanks for your interest in Charlix.

What's up Charlix?


 

* 06/28/2006 Charlix 0.3 is released, with some updates, I added a text to speech compatibility and a desktop creature working with AMOR. Say SPEAK to try.

* 06/01/2006  ReversedAIML has been released. It is an AIML file that transform simple text into AIML. Basically, it searches the verb and embeds possible questions about its subjects and objects in AIML code. 


* 04/23/2006  Charlix can be tested online. So now you can see what commands Charlix runs. And you can also take advantage of Charlix search features if popups are allowed for the pandorabot.com server.
Click here for Charlix Demo


* 04/22/2006  QAIML is released with charlix embedded
http://sourceforge.net/projects/qaiml
Charlix has been integrated in the binary version. 
So all you have to do to try charlix is to download QAIML, to run it and then to have fun... (see requirements also)
QAIML is a super nice job from Houssem BDIOUI. 

* 04/17/2006 Charlix 0.02 is released:
Added features:

the standard ALICE set has been integrated to Charlix, so now you can chat with charlix like to Alice.

A Bad answer editor in a Xdialog. 

Bug fixed for "bug report" and "feature request" will now be e-mailed. 
Divided system.aiml into several files: software.aiml, systemconfiguration, system.aiml, systemreduction.aiml and xtrareduction.aiml. 
Added new files: animation.aiml, hardware.aiml, kde.aiml, tutorialaiml.aiml.

 A lot of added categories. 
back to top of page


What is this?


Charlix is a desktop assistant. He is based in AIML from the implementation of Dr. Richard S. Wallace's A.L.I.C.E. system. For more information about the A.L.I.C.E. Foundation (including information on AIML, the Artificial Intelligence Markup Language), visit the A.L.I.C.E. homepage. 


If you ask Charlix for his MENU, he will answer:

Enter GAMES to look for games, 
INTERNET MENU will show you Internet search options, 
DICTIONARY (ANY WORD) : will show the definition of any word,
ask CALCULATOR, or I can CALC or CALCULATE (YOUR EQUATION)
PREFERENCES will help you to configure the default application to use, 
say BAD ANSWER whenever you want to teach me a better answer, 
FEATURE REQUEST to request what could be improved,  
and BUG to report a bug.
Of course ask anything or for any applications, I might be able to help.


Ask INTERNET MENU then Charlix proposes, 
MULTISEARCH (SOME TERMS TO SEARCH) : will display the result of 4 search engines,
BUY (SOMETHING) : will look on EBAY, 
ASKJEEVES (SOME TERMS TO SEARCH) : to look on ask.com,
GOOGLE or G (SOME TERMS TO SEARCH) : to look on google.com,
DMOZ (SOME TERMS TO SEARCH) : to look on dmoz, the open directory,
ASCII (SOME TERMS TO DISPLAY) : to make ascii art, 
WEATHER (ANY CITY OR COUNTRY OR ZIP CODE) : to get  a weather report,
NEWS : will check the news on BBC,
SUGGEST (SOME TERMS) : to look for a Customer Review on amazon.com,
TRANSLATE (a URL) to ask google to translate any web page into English, 
TRANSLATE IN (YOUR-LANGUAGE) (YOUR TEXT) to ask google to translate your text from English to your language, 
TRANSLATE FROM (YOUR-LANGUAGE) (YOUR TEXT) to ask google to translate your text from your language to English,   
Google supports: Chinese French German Italian Japanese Korean Portuguese and Spanish,             


back to top of page


What do I need to install?
 

 The system commands are all written for bash style shells. So it should work with *nix machines 
For DOS and other bill gates' XP fans. Charlix is probably possible to translate without much work. Or you can check HOWIE it is multi-platform bot based in python.


(If you think microsuck needs Charlix, send me an email I can help for the conversion.
but for sure Charlix does not need micro-nanosoft.)


You need an AIML interpreter with a working <system> tag. 
I have tested REBECCAAIML, LIBAIML and Qaiml .  CHATTERBEAN works if you replace all the: <system>  by: <system>exec(" and all the: </system> by: ")</system>

  Qaiml was the most compliant and user friendly. Charlix has been integrated in the binary version.
So all you have to do to try charlix is to download QAIML, run and have fun... (for security read my disclaimer.)
REBECCAAIML has about the same features, but comes as source. And your personal settings should be saved in a coming version.  Libaiml (latest unstable version) achieves almost everything but not CALCULATE (your equation), because the graphmaster ignores  signs like + - /and *; moreover you need to put  z in front of charlix specific files if you want them to have priority over others, because libaiml loads from z to a. 


 LYNX is used to get the weather, the news and send emails. 
 And Xdialog is now allowing interactiv wimdow dialogs. 


 You need to install FESTIVAL to get charlix voice the or another text to speech engine, thezn you need to edit the command in text2speech.aiml. AMOR (part of kde-toys) if you have KDE will give charlix a little desktop creature. But You need to install first the content of folder "charlix-amor" in the other creature directory "/usr/share/apps/amor" (on my debian box). 
 And Xdialog is now allowing interactiv wimdow dialogs. 


An included shell script "popup" (it checks your command is installed and fork it quietly with nohup) has to be in your path like for ex: /usr/local/bin and have the executable permission (embedded in QAIML). 

back to top of page


Can I trust Charlix?


Charlix has been tested. No major Bug occurred. But be aware that Charlix has as much power over your computer that you give it permissions.
So for security, It is advised to create a new user with read only permission, except for its own files. and to run the interpreter as this user. 
If you want the bot to learn, the aiml file update.aiml has to be with his.aiml files friends in a directory with write permission. So preferably install the files in your new user directory.
A fast way to do this as root:
# useradd -m charlix
to create a password
# passwd charlix
(enter twice a password)
# cd /home/charlix
Then install your favorite interpreters files in this directory.
change ownership of the file to charlix and group 1000 (first user): # chown charlix:1000 /home/charlix 
Start charlix with sux (to sudo under x)
$ sux charlix "command_to start_your_interpreter"
or with kdesu like $ kdesu -u charlix -c "/home/charlix/QAIML/QAIML" (in KDE with QAIML)

You can run charlix normally but you are the only person to blame, if anything is damaged or erased on your system. By the way the GNU license of this package specifies:

 BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY
FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT WHEN
OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES
PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED
OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS
TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE
PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING,
REPAIR OR CORRECTION.



back to top of page


How do I configure charlix?

Copy those predicates (look at the bottom of the page too) in your bot property file after you adapt those predicates to your needs:

<set name="aimlpath">./aiml</set>(this is the absolute or relative path of your aiml files
<set name="desktopenvironment">gnome</set>
<set name="instantmessenger">gaim</set>
<set name="emailclient">evolution-2.0</set>
<set name="organizer">evolution-2.0</set>
<set name="phoneclient">gnomemeeting</set>
<set name="console">gnome-terminal</set>
<set name="browser">mozilla</set>
<set name="filebrowser">nautilus</set>
<set name="calculator">gcalctool</set>
<set name="dictionary">gnome-dictionary</set>
<set name="wordprocessor">abiword</set>
<set name="spreadsheet">gnumeric</set>
<set name="presentationeditor">ooimpress</set>
<set name="imageeditor">gimp</set>
<set name="helpbrowser">yelp</set>
<set name="musicplayer">xmms </set>
<set name="movieplayer">xmms </set>
<set name="cdburner">gnomebaker</set>
<set name="texteditor">gedit</set>
<set name="accountability">gnucash</set>
<set name="archiver">file-roller</set>
<set name="softwareinstaller">gksu -u root /usr/sbin/synaptic</set>
<set name="picturewiewer">eog</set>
<set name="volume">kmix</set>
<set name="searchengine">google</set>
<set name="logfile">./logfile</set>
<set name="documentfolder">/home/<get name="username"/></set>
<set name="picturefolder ">/home/<get name="username"/></set>
<set name="videofolder">/home/<get name="username"/></set>
<set name="musicfolder">/home/<get name="username"/></set>
<set name="networkfolder">/home/<get name="username"/>/mnt</set>
<set name="aimlpath">./aiml</set>
<set name="username">username</set>
<set name="aimlpath">./aiml</set> AIML files path
<set name="interpreter">interpreter</set> process name for example console or QAIML


Or you can ask PREFERENCES, then you will be prompt for those predicates.


Put the AIML files in the correct directory. annoted_alice for Rebeccaiml. The files  therebeccaconnect.aiml should be added if you use Rebecca, kde.aiml if you want help about kde.

Include the icon charlix.xpm in you aiml engine working directory. 

Now you are ready to have fun with Charlix. Start your interpreter.

back to top of page


How can I get help?

  
You don't understand? I does no work?
If you have any concerns or Charlix is not nice.
Tell me your pains and misery on chcdsl(cat)yahoo(dog)fr
If you want help, use "bad answer" and send your updates. (ask preferences)  
Or even send me a log of your conversations. 
If you know c++ I need help to make a GUI, I'm building.
chcdsl(cat)yahoo(dog)fr



back to top of page

How can I personalize Charlix?

Use your imagination and include this in the BOT properties.

    <set name="name">Charlix</set>
    <set name="botmaster">developper</set>
    <set name="master">You buddy</set>
    <set name="genus">penguin</set>
    <set name="location">Antarctica</set>
    <set name="gender">male with a small *</set>
    <set name="species">penguin</set>
    <set name="birthday">November 23, 1995</set>
    <set name="order">penguin</set>
    <set name="party">Open Mind Party</set>
    <set name="birthplace">Antarctica</set>
    <set name="president">Dummy penguin</set>
    <set name="friends">Everybody</set>
    <set name="favoritemovie">March of the penguins</set>
    <set name="religion">GNU</set>
    <set name="favoritefood">fish</set>
    <set name="favoritecolor">snow-white</set>
    <set name="family">penguin</set>
    <set name="favoriteactor">William Hurt</set>
    <set name="nationality">Antartican</set>
    <set name="kingdom">Antartican</set>
    <set name="forfun">fish</set>
    <set name="version">MARCH 2006</set>
    <set name="sign">Sagittarius</set>
    <set name="phylum">Computer</set>
    <set name="friend">HOWIE</set>
    <set name="website">http://charlix.sourceforge.net/</set>
    <set name="talkabout">artificial intelligence, robots, art, philosophy, history, geography, politics, and many other subjects</set>
    <set name="looklike">a penguin</set>
    <set name="language">English</set>
    <set name="girlfriend">no girlfriend</set>
    <set name="favoritesport">Hockey</set>
    <set name="email">chcdsl (cat) yahoo (dog) fr</set>
    <set name="age">8</set>
    <set name="wear">my feathers</set>
    <set name="vocabulary">10000</set>
    <set name="question">What's your favorite movie?</set>

