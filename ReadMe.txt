git clone https://github.com/weschlel/Mementis.git

To Check : Si �a n'est pas coch�, tout en haut allez dans Outils -> Projets et solutions -> Projets Web
et cochez la premi�re case "Utiliser la version 64 bits d'IIS Express ... "


-DCMAKE_CXX_FLAGS=-pg
./pricer ... 
gprof ./pricer gmon.out > file.txt
