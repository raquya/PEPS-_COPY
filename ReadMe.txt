git clone https://github.com/weschlel/Mementis.git

To Check : Si ça n'est pas coché, tout en haut allez dans Outils -> Projets et solutions -> Projets Web
et cochez la première case "Utiliser la version 64 bits d'IIS Express ... "


-DCMAKE_CXX_FLAGS=-pg
./pricer ... 
gprof ./pricer gmon.out > file.txt
