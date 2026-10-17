
## Application SPILM35Ex.ino

l'application ici présente à pour but de corriger un problèmes à l'intérieur du code Arduino de la même branche (LGT8F328), ou l'application LGT8F328/ST7735RotaryBtnEx/ST7735RotaryBtnEx.ino(1) est équipé d'un communicateur I2C, un bouton poussoir et rotatif et un écran SPI de la même nature que l'application ici. Il y a un léger bug au niveau de la prise de température et après avoir développé le laboratoire pour expliquer le fonctionnement du LM35DZ. Ce n'est pas le bon matériel nécessaire pour capturer via une sonde avec LM35DZ à vrai dire sur le marché de détail il n'y a pas de sonde à base de LM35DZ, il n'y a que les thermistors et le DS18B20 qui ne font pas encore objet de tests. Et ayant ajouté plusieurs commodités à la communication I2C qui semble bien répondre avec une interface python et d'un ESP32 avec micro-python, ici il n'y a que la démonstration d'un temps d'arrêts et de lecture. Ainsi qu'une analyse de la charge et temps de charge en coulomb d'un pont diviseur à base de résistance et de condensateur. j'ai des images d'une résistance trop base et les effets sur le temps à ajouté qui influent aussi sur la température du montage. Possible qu'une mauvaise configuration dans le schéma de montage puisse indiqué la température de l'élément qui force le plus et non juste la température idéal dans environnement. La faute d'indiqué un si haut voltage serait en fait la température et le stress soumis à la pièce qui aliment ma planche à pain, le LDO(2), ou le ams-1117 en format 3v3 et 5v à la droite sur l'image (La planche à pain avec l'écran). Une prise de température avec un appareil à guidage laser montre les deux pièce être supérieur à 40 degré Celsius et serait le stress final pourquoi le LD35DZ indique la même température. 


Table des matières :

 1. Le LM35 et sa condition.
 2. Description de l'activité.
 3. Liste du matériel.
 4. Plan de construction Arduino à Suivre.
 5. Outils nécessaire pour la réalisation de l'application.
 6. Les détails de réalisation.
 7. 
 8. 
 9. 
 10. 

## 1. Le LM35 et sa condition.

Ici, vous voyez 2 LM35DZ côte à côte pincé par une simple pince qui colle l'étiquette de définition d'une puce en format TO-92. Alors face à face sur un fil de 10 cm en dehors d'une planche à pain à donné de très mauvais résultat lorsqu'ils ont trempés dans la glace et ne point descendre de quelques dégrée Celsius. De plus dans une condition inverse de le tenir près d'une flamme de bougie et dans la cire fondu n'ont pas vraiment changé les résultats. À quoi sert le LM35 ? 


![enter image description here](https://raw.githubusercontent.com/priendeau/Arduino/refs/heads/main/LGT8F328/SPILM35Ex/SPILM35Ex_dual-lm35_small.png)

Une fois le diagramme interne inspecté il n'y a que la présence d'une de deux transistors et une ligne de sortie qui garantie via une mini-monitorage de l'ampérage garantisse la sortie à 8.8mV/Celsius et une autre comparateur qui garantie d'élevé la tension à 10mV/Celsius. Alors il s'agit d'un convoyeur d'énergie qui garantie que dans le cas de un transistor 10 fois plus puissant que le transistor de base à eux deux garantissent une sortie final à 10mV/Celsius. Nous parlons ici d'une analyse de flux électrique qui dépend des facteurs extérieur et pratique une compensation avec les comparateurs A1 et A2. De plus il y a une implémentation de pont de résistance Wheatstone(3) entre la série nR1 et ( .125 R2 + R2 ) qui égalise la différence pour calibrer la sortie.   


![À gauche, Spécification LM35 à l'intérieur, À droite, pont Wheatstone](https://raw.githubusercontent.com/priendeau/Arduino/refs/heads/main/LGT8F328/SPILM35Ex/LM35_Internal_schema.png)

Ici dans le diagramme il y à gauche le schéma interne du LM35 et à droite le pont Wheatstone(3) qui fait office de comparaison pour obtenir la vrai valeur en Ohm de Rx est par l'analyse du courant qui passe entre le pont de division A - ( pont de division C ) où R2 est variable pour l'atteinte de la moitié du voltage et connaître la valeur de Rx.  


## 2. Description de l'activité.

L'activité à pour but de qualifié si vous cherchez de prendre la température via une sonde ou une prise générale de la température. Vous pouvez passez à travers une grande quantité de LM35DZ, les 5 spécimens testé donne des résultats plus haut que prévue, mais quelque fois donne vraiment la bonne température. Les conditions lors de l'écriture de l'activité était en automne et les journées froides et sombres sont plus fidèle de reflété une température au thermostat et les journées humide et ensoleillés ont donnés des résultats élevés. Cependant entre la section ombragé de la maison et la section sous-sols donne des résultats élevés et pourtant le solarium ou il devrait faire la température indiqué sur l'écran semble valide, mais n'a pas changé de son démarrage dans le sous-sol. Alors les conditions internes sont absolument géré par la présence du soleil et l'énergie total disponible dans environnement sous ensoleillement. Il va de pair que la méthodologie à l'intérieur du LM35 ne fait qu'un calcul de d'évidement entre un gros transistor 10x plus puissant que celui qui récrimine et réalimente la valeur de base à 1.38 volts (voir le schéma LM35DZ). 

### Domaine d'hypothèse. 

Comparé avec une recharge sans-fil appelé par induction certaines montre moderne nécessite une recharge par induction et sont extrêmement chaude suite à l'induction du voltage et le capteur de voltage. Descendre la chaleur résiduelle pour éviter que la montre soit brûlante, implique que le chargeur devrait être équipé d'un évaluateur de passage d'énergie et entre dans le domaine du LM35. Travaillant avec le phénomène de charge et d'impulsion une charge rapide impliquerait aussi d'injecté à plus haute fréquence l'impulsion et génèrerait plus de courant en passage. Le LM35 conditionnerait l'empêchement d'atteindre des valeurs qui brûle l'utilisateur ou de faire fondre le plastique de la montre.
  

Le LM35DZ, celui qui ne dépend pas de la calibration du voltage de Vout et Gnd (appellé LM35) qui donne en autre des valeurs en Kelvin. le LM35DZ se connecte à 4 Volts et supporte une tension au plus de 30V. La sortie sortie étant unilatéralement entre 55mV à -55 et de 1.00 Volt à 100 Dégrée Celsius peut être connecté à un microcontrôleur opérant à 5V ou 3V3, mais ne peut fonctionné avec le voltage du 3V3, rapporte encore plus de différence avec un microcontrôleur 3V3. L'astuce est de garantir un voltage d'au moins 4 volts. Il n'y a pas que le LM35DZ qui demande des contraintes similaires. Le DHT11, AMT22 sont aussi avec le même régime d'alimentation qui est au moins 4Volts. 

L'activité proposé ici donne 2 avantages, l'utilisation du transistor NPN(4) pour basculer du voltage secondaire à base de 5 Volts et de calibrer la sortie avec un   

![La planche à pain avec l'écran](https://raw.githubusercontent.com/priendeau/Arduino/refs/heads/main/LGT8F328/SPILM35Ex/SPILM35-main-screen-3.png)

### Description de l'image

Ici dans l'image de "La planche à pain avec l'écran", il y a au centre l'écran SPI(5) ST7735 à 80x160 qui marche à l'envers, il y la fonction spécial qui prévoit économisé une Pin de sur le microcontrôleur et utilise conjointement la Pin "Res" sur l'écran est connecté sur le Reset du microcontrôleur. En arrière de l'écran il y a la partie pont de division de courant à base de 2 résistance et de deux condensateur. À gauche du pont principal il y a les deux LM35DZ cadencé avec un transistor ici non visible à cause de la pair de fils violet-bleu-vert vous ne pouvez voir que la résistance de 470 Ohms en dessous de la pair de fils.  À droite totalement sur la planche à pain, la barre d'alimentation avec les ams1117 en format 3V3 et 5V. Le tout est relié à un paquet de batterie 18650 qui sont convertis en alimentation compatible USB ou un 5V. Il va de même que l'alimentation jusqu'à 12V est inclus et donne le même résultats sinon un plus grand décalage à cause qu'il y a plus de stress pour convertir la tension en 3V3 et 5V. Noté que le rail du haut ou jonction de courant du haut est à 5V et celle du bas à 3V3. le microcontrôleur LGT8F328 supporte le 3V3 comme le 5V. Le cas échéant nôtre microcontrôleur est un 3V3 et ne peut alimenté directement les LM35DZ. De plus dans l'application la dernière ligne avec le Vref ou il est indiqué 5147mV correspond à une approximation avec le résultat du premier pont monitoré avec la branche A0.  
 
### Précaution à prendre pour la substitution d'un transistor NPN(4).

Le montage initiale est bel et bien avec un transistor 2n2222, qui est polyvalent et est disponible chez Addison Électronique, ou en ligne dans un centre de détail. Aliexpress(6) offrent de plus en plus des transistor de type US, mais inclus librement les transistor Internationaux ou Chinois tel le BC337. Il y a une petite différence. Tous les deux partagent la base sur la pin numéro 2, mais le Collecteur et l'Émetteur sont inversé dans le cas du BC337 donc lorsqu'il mentionné de connecté l'Émetteur à la base pour le 2n2222 il s'agit de la pin 1 à la gauche lorsque le transistor affiche la face plate vers vous et dans le cas du BC337, BC547, BC548, BC549, BC550, la pin Émetteur est la dernière à droite. Pour substitué le 2n2222 vous pouvez le remplacé par le 2n4401 qui est aussi NPN. Le schéma présenté pour connecté le LM35DZ à un transistor NPN(4) est différent d'un schéma PNP(7) puisque la définition du NPN(4) est de bloqué la base du courant pour y laisser fuir ( phénomène d'écoulement ou "Sinking(8)" ) l'électricité vers l'émetteur qui va vers la base ou "Ground" en anglais.

![À gauche le transistor 2n2222. À droite le transistor BC337](https://raw.githubusercontent.com/priendeau/Arduino/refs/heads/main/LGT8F328/SPILM35Ex/2N2222-Pinout.jpg)


## 3. Liste du matériel.

**Schéma initiale**
|     Nom       | Quantité |           Description des éléments              |        
|---------------|----------|-------------------------------------------------|
|    U1         |    1     | LGT8F328 QFN32 sur plaquette                    |
|    U2         |    1     | ST7735 80x160 pixel 65k/282k couleur            |
|    U3         |    2     | LM35DZ                                          |
|    U4         |    1     | FTDI-232 USB/TTL loadeur                        |
|    T1         |    1     | 2n2222 NPN transistor ou le BC337               |
|    C1         |    1     | 470 uF, 6.3 V condensateur polarisé             |
|    C2-C3      |    2     | 100 uF 6.3-25 V condensateur polarisé           |
|    C4-C5      |    2     | 10 nF 6.3-25 V condensateur céramique           |
|    R1-R3      |    3     | 1 k Ohms Résistance 1/4 de watt                 |
|    R3-R4      |    2     | 4k7 Ohms Résistance 1/4 de watt                 |
|    R4-R5      |    2     | 10 ou 20 k Ohms Résistance 1/4 de watt          |
|    R6         | 1 ou 3   | 470 Ohms Résistance 1/4 de watt                 |
|    P1         | 1 ou 0   | Planche à pain avec rail (facultative)          |
|    D1         | 1 ou 0   | Dérive de courant à 3V3 et 5V.                  |

**Expérience secondaire**

|     Nom       | Quantité |           Description des éléments              |        
|---------------|----------|-------------------------------------------------|
|    U5         |    1     | AMS-1117-adj ou LM317-ADJ                       |
|    R6         |    1     | 10 k Ohms Résistance 1/4 de watt                |
|    R7-R8      |    2     | 200 Ohms Résistance 1/4 de watt                 |

## 4. Plan de construction Arduino à Suivre.

Le schéma ici présent gracieuseté "Cirkit Designer" est gratuits et si les pièces ne sont pas disponible dans la liste des composantes, vous pouvez les ajoutées avec une photo et une reconnaissance des bornes de connections. Ici présent la plaquette microcontrôleur LGT8F328 à été ajouté manuellement et le code via le lien fournis pour visiter aussi le site en question accepte mais ne peut faire de simulation puisque le c\u0153ur lgt8fx:avr de DBUEZA(9) n'est pas ajouté ou testé. 

Les contacts sont en l'air, et est une représentation imagé entre un vrai plan électronique de type ingénieur, mais tous les contacts sont présent. Vous pouvez établir une relation de contact-en-l'air vers planche à pain sans problème. C'est que la représentation planche à pain n'est pas fidèle à la réalité et il faudrait une très grosse planche à pain pour ajouté les 3 ponts de divisions. Et il n'y a tellement pas beaucoup de connexions que la lecture via planche à pain est fastidieuse.

### représentation du schéma


![](https://raw.githubusercontent.com/priendeau/Arduino/refs/heads/main/LGT8F328/SPILM35Ex/SPILM35Ex_main_circuit.png)
Reference: https://app.cirkitdesigner.com/project/517cf01c-8466-4958-a5d0-4e3969cfa1ed

### Section du montage
- Pont Diviseur 1
	- Relié à la branche A0, sert d'analyse de la tension.
	- Doit rapporté la moitié du voltage puisque le code comprend une lecture et re-valide le la tension total à l'instar de la valeur par défaut.
	- Une des raisons pourquoi il est conseillé de connecté une résistance de 10k ou 20k, plus petite elle brûlera avec le temps. Voir les détails de réalisation. 

- Pont Diviseur 2 et 3
	- Alimente le LM35DZ 1 et 2
- Transistor de contrôle
	- Est le déclencheur qui active ou met en veille les LM35DZ 1 et 2.

## 5. Outils nécessaire pour la réalisation de l'application.

Vous pouvez re-visitez la branche de mon github.com Arduino/LGT8F328/ST7735RotaryBtnEx pour les explications d'installation de arduino-cli(10), de la fonction arrayBuilder(11) et arduino-compile(12). Ici il n'y a que l'ajout visuel du arrayBuilder(13) permettant la visualisation avec zenity disponible sous Linux et quelques distribution compatible Posix/Unix(14), mais zenity étant grandement disponible dans plusieurs distribution Linux, Windows pourrait supporté zenity à travers Mingw32/64(15) et Msys(16) qui semble toujours disponible pour Windows 11. Cependant Linux est fortement conseillé et OSX(17) reste plus ou moins viable, mais à tous terminal Bash(18) disponible il est possible de continuer. 

Ils y a quelques outils non mentionnés qui reste indispensable à la vérification de vôtre montage et vérification de certains pré-requis. 

Adaptateur prérequis pour envoyer du code binaire au microcontrôleur 

![FTDI2232 USB/TTL convertisseur ](https://raw.githubusercontent.com/priendeau/Arduino/refs/heads/main/LGT8F328/SPILM35Ex/ftdi_232.png)

Paquet de batterie avec support USB(19). Cette version cheap acquis sur AliExpress est correct. Offre une entrée de charge en USB micro et type-C. Offrent 2 sortie USB-A, utilise 3 batteries 18650. Cependant les 3 batteries en main sont des 10000mAh et la charge interne à ce paquet n'est pas fidèle à la recharge. L'avantage de petites lumières sur le dessus pour éclairé vos montage dans le noir si vous avez peu de lumière sur vôtre place de montage favoris. 

![paquet de batterie rechargeable, USB](https://raw.githubusercontent.com/priendeau/Arduino/refs/heads/main/LGT8F328/SPILM35Ex/batterie_pack.png)

Un multimètre. Indispensable. Le pont de division de voltage pour chacun  des LM35DZ doit être de 4.0 volts. La configuration présenté sur le schéma est de 1.00 Kilo Ohms et  4.7 Kilo Ohms sur une base de 5 volts. 

Le calcul est **Vout = (Vs * R2)/( R1 + R2 )** ou:

| Symbole| Définition        |
|--------|-------------------|
|Vout    | Voltage sortant   |
|Vs      | Voltage source    |
|R1      | Résistance 1      |
|R2      | Résistance 1      |

La réponse pour le groupe 2 et 3 du pont de division de voltage devrait doné 4.123 volts, cependant au multimètre ça indiquait 4.0 Volts et quelque fois 4.1, mais pas en dessous de 4.0 volts.       
![Multimètre ](https://raw.githubusercontent.com/priendeau/Arduino/refs/heads/main/LGT8F328/SPILM35Ex/multimeter.png)

Deuxièmes fonction importante dans le multimètre est **DC A**. Permettant de réalisé le test suivant qui consiste à alimenté les LM35DZ avec un contrôle d'ampère précis avec le AMS1117-adj et une 10k Ohms et 2x 200 Ohms pour obtenir un jeux de 120u Ampère, le multimètre présent peut indiqué sous la sélection 2m dans la section **DC A** indiquera 0.012 à 0.010 ce qui est apréciable comme précision.
 
### Logiciel à utiliser
 Le logiciel en mode texte ou terminal du type arduino-cli est important. Dans la version développé ici est conçu pour utiliser une table de #define(20) qui ont tous une valeur par défaut. En utilisant l'application arrayBuilder en mode graphique vous pouvez extraire les valeurs des différents #define et les transposé dans Arduino-IDE(21) vous même. Ici il y a des valeur à changés et implique une ou plusieurs recompilation de l'application et la transmettre à nouveau au microcontrôleur. il vous faudra aussi avrdude(22) qui est important à utilisé. Tous est déjà expliqué dans la branche de l'application  précédente LGT8F328/ST7735RotaryBtnEx 

### Utilisation visuel de arrayBuilder 
Ici, il y a une mise à jour de arrayBuilder vous permettant quelques opération via des fenêtres modal à réponse et a résultat. La fonction n'est pas encore officiellement fini en mode visuel, mais permet l'utilisation à travers le terminal.

Dans le cadre de visualisé les #define ajouté à une ligne standard de compilation via arduino-cli vous pouvez tout capturer ainsi:

Chargez la fonction arrayBuilder via l'appel de script bash. À l'intérieur il y a arrayBuilder et arduino-compile 
**bash**

    . ./aliasrc
    . ./Arduino.sh
     

Renommez le fichier BuildProperty par .BuildProperty
**bash**

    mv BuildProperty .BuildProperty
Le fichier va virtuellement disparaître mais sera disponible si vous utilisez 'ls(23)' avec l'option de tout afficher.

Chargement Pré-requis:

**bash**

	load_BuildProperty

Lancement d'arrayBuilder en mode visuel.

**bash**

    ACTION=view BPUseGui=True BPArrayName=BuildProperty BPFileConf=.BuildProperty arrayBuildProperty

Suivez les étapes du haut vers le bas et de gauche à droite.

![Vue de arrayBuilder ](https://raw.githubusercontent.com/priendeau/Arduino/refs/heads/main/LGT8F328/SPILM35Ex/arrayBuilder_view_all_windows.png)

À chaque sélection il faut appuyer sur (OK), à l'exception de la première fenêtre ou il y a le choix "Compile switch" et "Build Property". À l'exception du ficher .BuildProperty qui appartient à arduino-compile. Vous pouvez vous même créer d'autre tableau associatif appellé "Associative Array" et seront aussi visible avec arrayBuilder si vous changez les variables BPArrayName & BPFileConf dans la ligne d'appel de arrayBuilder. La variable BPArrayName est requis pour le nom de vôtre tableau à visualiser et si vous n'avez pas fait de sauvegarde physique d'un Tableau associatif sous le terminal Bash, il faut le crée manuellement. 

Puisque arrayBuilder est conçu pour travailler avec arduino-compile, la consultation d'un tableau BuildProperty offres 2 choix, "Compile switch" et "BuildProperty", dans le cas d'un Tableau qui ne s'appelle pas BuildProperty dans la même fenêtre vous allez voir "Compile switch" où toutes les nom d'indice commençant par "--" sera visible sous "Compile switch" et le reste via "Index". 

À noté dans la fenêtre "BuildProperty view Windows" tous les paramètres sont écrit un en dessous de l'autre et ne reflète pas la définition dans le fichier et accessible via l'accès direct à BuildProperty['compiler.cpp.extra_flags'] et BuildProperty['compiler.c.extra_flags']

### Créer à des fin de tests un Tableau associatif.

**bash**

    declare -A MonTableau
    MonTableau['--option1']=None
    MonTableau['--option2']=Rien
    MonTableau['compiler.cpp.extra_flags']='-O0'

Sauvegardé MonTableau 

**bash**

    ACTION=save BPUseGui=False BPArrayName=MonTableau BPFileConf=.MonTableau arrayBuildProperty 

Et apparaîtra un fichier du nom ".MonTableau" dans la liste de ficher. Celui-ci sera accessible via la même commande arrayBuilder avec les bons paramètres :

**bash**

    ACTION=view BPUseGui=True BPArrayName=MonTableau BPFileConf=.MonTableau arrayBuilder
 
## 6. Les détails de réalisations.
 
 Lors de la connexion de vos composante électronique il est fortement conseillé de le faire en mode hors tension, puisque le microcontrôleur lorsqu'il est configuré avec la programmation il est prêt et ne vous attend pas. Il donnera des informations flous si vous connectez le tous sous tension. Bien identifié la trame de courant. Dans le cas de la planche à pain c'est la ligne de pouvoir qu'il faut change avec des cavaliers. Celle du haut à été configuré à 5V et celle du bas à 3V3. La puce LGT8F328 supporte le 3V3 comme le 5V, mais il faut lire le LDO à côté de la plaquette et c'est cette composante qui décide, dans le cas choisi elle reste à 3V3 même si les 2 trames sont en 5V.  
 
### Pourquoi le Pont diviseur 1 doit être précis.

À cause dans la programmation de de laboratoire l'estimation du voltage est une réalisation d'une fraction qui implique de multiplier par 2 le résultats. Alors il faut faire des efforts avec le multimètre pour trouver dans vôtre paquet 2 résistance de 10 k Ohms qui indique 10.0 k Ohms à la lecture du multimètre. Cependant, vous pouvez classifiés le groupe de résistance qui sont généralement collé sur papier et prendre une paire similaire tel 9.85 k Ohms ou 10.3 k Ohms. Il est stipulé que les résistance que vous achetez ont une tolérance de 10% vis-à-vis ce qu'elles sont. Alors si vous trouvé 9.75 k Ohms ou 10.5 k Ohms elle ne sont pas les meilleurs, mais si vous avec 2 résistances avec le même résultat hors tolérance, vous avez de la chance. 
    
### Pourquoi ça brûle ?

Un exemple de résistance qui brûle sous le stress. 

![résistances brûlés ](https://raw.githubusercontent.com/priendeau/Arduino/refs/heads/main/LGT8F328/SPILM35Ex/burn_10ohms_compare.png)

Il s'agit de résistance de 10 Ohms et sous le 3V3 elles ont brulées. Elle indique encore 10 Ohms, mais suite à une question de qu'est-ce qui chauffe et qui sent le plastique qui chauffe. Il s'agissait de résistance qui brûle. La raison pour leur utilisation provient d'une anomalie observée. Mais dans ce cas présent, la résistance est trop base.

### Si la case Vref sur l'écran n'indique pas le bon voltage ?

![voltage n'est pas fixe ](https://raw.githubusercontent.com/priendeau/Arduino/refs/heads/main/LGT8F328/SPILM35Ex/anomaly_volt_low.png)

Il s'agit aussi de la même anomalie. Anomalie sur la résistance utilisé et anomalie sur la tension calculé. Celle si apparaît lorsque vous ajoutez à l'intérieur du "pont diviseur 1", une grosse résistance qui prend du temps à charger les 2 condensateurs de cette partie. D'ailleurs ensemble utilisé des résistance base pour accélérer la charge à sa valeur nominale et éviter que la résistance brûle via une plus grosse valeur sont directement lié. Alors dans ce cas si si la valeur de la résistance est 20 k Ohms combien de temps ça peut prendre ?

Aussi importante comme loi, celle du temps de charge  d'un condensateur explique qu'à 10 Ohms la valeur en temps est de 0.00100 secondes mais risque de brûler.  Il faut connaître l'équation de temps de charge et d'énergie impliqué pour la charge. Voici la première équation le temps de charge:

Équation:  **T=RC**

| Symbole| Définition                  |
|--------|-----------------------------|
|   T    | Temps de charge             |
|   R    | Résistance dans le réseau   |
|   C    | Capacitance associé         |

Alors pour 1 condensateur de 100uF et une résistance de 20 K Ohms prendra 2.00 secondes
Pour 1 condensateur de 100uF et une résistance de 10 K Ohms prendra 1.00 seconde
Pour 1 condensateur de 100uF et une résistance de 470 Ohms prendra 0.0470 secondes

Dans le cadre de l'application. les valeurs de base à l'intérieur du Tableau BuildProperty sont indiqué qu'il y a un nombre d'essais et un délai à chaque tentative de lecture. voici les variable:

À l\u2019intérieur de l'index compiler.c.extra_flags et compiler.cpp.extra_flags : 
**bash**

    view_BuildProperty
    
Recherché -DSAMPLING_READ et -DSLEEP_READING

Réponse:
**bash**

    -DSAMPLING_READ=500 -DSLEEP_READING=30

Il y a 500 lectures qui constitue le phénomène de statistique standard du nombre d'échantillons collecté pour avoir un résultats décent. Dans l'application présente ou à la ligne Vref(24) présente 2584.81 mV n'avait que 100 lecture et la moitié de la valeur de repos. Donnant comme variable:

**bash**

    -DSAMPLING_READ=100 -DSLEEP_READING=15

En gros la boucle principale à l'intérieur de loop() présente une section qui prend l'échantillonage de Vref et de la branche A1 et A2 toute en même temps, à la fin de la boucle sans compter le temps que la fonction AnalogRead(...) enregistre les valeurs. Il se passera:

- À 500 valeurs et 30ms de pause par échantillonage .
	- Délai de 15 secondes. 
- À 100 valeurs et 15ms de pause par échantillonage .
	- Délai de 1.5 secondes.   

Il y a 2 condensateurs à chargés donc:

- À 10 K Ohms , le temps de charge passe à 	2.00 secondes.
- À 20 K Ohms, le temps de charge passe à 4.00 secondes

Alors, 500 valeurs sont court pour considérer la première valeur intacte. Autre possibilité. Dans la section setup() ou commence la déclaration des Branche à configurer en mode LECTURE ou ÉCRITURE, il faut ajouté plus de temps à la commande delay() pour obtenir une valeur uniforme ou le microcontrôleur aurait déjà eu le temps nécessaire pour stabiliser la charge et commencer à opérer. N'oubliez pas Les ponts de division 1 à 3 sont directement connectés au voltage de de la planche à pain. Mais aussi c'est le transistor qui dépend du microcontrôleur et est déconseillé de monitoré le voltage de l'un des pont 1 et 2 puisqu'ils sont réveillé avant la pratique de l'échantillonage et endormis de 1 à 3 secondes après échantillonnage. 

Dans la liste du matériel il est inscrit un nombre supplémentaire de résistance de 470 Ohms, pouvant être une alternative. Combien de temps de charge cela prendra-t-il ?   

### Énergie de charge des condensateur du pont diviseur 1

Quel énergie est impliqué dans la charge des 2 condensateurs, et est-ce que l'utilisation des résistances change quelque chose ?

À l'équation **E=CV^2/2** qui est l'énergie impliqué dans la charge d'un condensateur, il n'y a pas de valeur associé à **R** pour la résistance la réponse devrait être généralement non ! 

Équation **E=CV^2/2**

| Symbole| Définition                  |
|--------|-----------------------------|
|   E    |  Énergie total en Coulomb   |
|   C    |  Capacitance en Farad       |
|   V    |  Voltage appliqué           |


Énergie utilisé:

- À 10 Ohms, 470 Ohms, 10 et 20 k Ohms la réponse reste ( 0.000544 * 2 ) pour 2 condensateurs .    
- 0.001088 Coulombs, qui sont apparemment trop élevé pour supporté une charge de 0.00100 secondes sur deux résistances à 10 Ohms. Petit ajout les résistances utilisés sont des 1/4 de watt.

Selon la définition de Watt qui représente Joule par seconde (J/s) Si le E de l'énergie en coulomb est relationnel, alors le nombre de Watts pour 2 résistances d 10 Ohms se calcul ainsi:

0.001088 Coulombs / 0.00100 secondes = 1.088 Watts et des résistance en série reste avec le même potentiel de résistance 1/4 Watts, soit le montage du pont de division 1 ici présent prouve que l'effort génère 1.088 Watts ce qui est beaucoup pour 2 résistances en série de 1/4 de Watt.
 
## 7. 
[PARA]

## 8. 
[PARA]
## 9. 

## 10. 
[PARA]

## Réference

note:
-	(1) : LGT8F328/ST7735RotaryBtnEx/ST7735RotaryBtnEx.ino ou
	-	LGT8F328/ST7735RotaryBtnEx font référence à https://github.com/priendeau/Arduino/tree/main/LGT8F328/ST7735RotaryBtnEx
- (2) : LDO : Pour "Low-Drop-Out voltage componnent".
- (3) : Wheatstone       : Un pont de Wheatstone est un circuit électrique utilisé pour mesurer une résistance électrique inconnue en équilibrant deux branches parallèles formant des diviseurs de tension.
- (4) : NPN              : Un transistor NPN est un type de transistor à jonction bipolaire (BJT) formé en intercalant une fine couche de semi-conducteur de type p entre deux couches de semi-conducteur de type n.
- (5) : SPI              : L'interface série périphérique (SPI) est un protocole de communication série synchrone développé par Motorola dans les années 1980.
- (6) : Aliexpress       : Site web de marché en ligne pour divers produits.
- (7) : PNP              : Un transistor PNP est un type de transistor à jonction bipolaire (BJT) formé en intercalant une fine couche de semi-conducteur de type n entre deux couches de semi-conducteur de type p.
- (8) : Sinking          : Méthode découlement de l'énergie à travers un transistor. Il y a alimentation à la source ou reflux via la base.
- (9) : DBUEZA           : github pour utilisateur qui a dévelopé le code pour la famille LGT8FX, https://github.com/dbuezas 
- (10) : arduino-cli     : application Open Source disponible sous Windows, Mac, Linux pour compiler du code pour microcontrôleur
- (11) : arrayBuilder    : fonction Bash sous Linux personnellement développé.
- (12) : arduino-compile : fonction Bash sous Linux personnellement développé utilisant arrayBuilder pour compiler du code pour microcontrôleur . 
- (13) : zenity          : application Linux sous Gome/XWindows/Kde à interface modale configurable pour intéraction avec l'utilisateur.
- (14) : Posix/Unix      : L'interface POS (Portable Operating System Interface) est une famille de normes spécifiées par l'IEEE Computer Society pour assurer la compatibilité entre les systèmes d'exploitation.
- (15) : Mingw32/64      : API de développement quasi POSIX émulant Linux sous Windows depuis Windows 98. 
- (16) : Msys            : Système Unix d'interface pour la gestion matériel et des fichiers similaire à Bash sous Windows depuis Windows 98.
- (17) : OSX             : Système d'opération pour produit Appel . 
- (18) : Bash            : Provenant de "Bourne Again SHell", est un interpréteur de commandes interactif et un langage de commande développés pour les systèmes d'exploitation de type Unix. Soutenu par la Free Software Foundation et créé en 1989.
- (19) : USB             : Universal Serial Bus.
- (20) : #define         : Relatif au langage de programmation C/C++.
- (21) : Arduino-IDE     : L'IDE Arduino (environnement de développement intégré) est un logiciel open source utilisé pour écrire, compiler et téléverser du code (appelé « sketches ») vers des cartes Arduino et des cartes à microcontrôleur compatibles.
- (22) : avrdude         : AVRDUDE (AVR Downloader/Uploader) est un utilitaire en ligne de commande gratuit et open source utilisé pour programmer les mémoires intégrées des microcontrôleurs AVR de Microchip (anciennement Atmel).
- (23) : ls              : Commande sous Linux sh / bash, défile la liste des fichiers et génère de l'information associé aux fichiers tels l'espace requis.
- (24) : Vref            : Diminutif pour Voltage Référence.


