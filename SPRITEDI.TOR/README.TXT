#
# DOC DE l'EDITEUR DU SPRITE
#

L'ecran est decompose en 2 parties :
 o Le haut de l'ecran contient une barre de menu qui permet de
    o charger une image TGA 24 bits en 320*200. (direct from
      apex) (TGA> en haut a gauche)
    o charger un raw 16 bits en 320*200. (RAW> en haut a gauche)
    o sauvegarder un raw (donc on peut convertir facilement des
      TGA en raw) (>TGA vers le milieu)
    o capturer un sprite.
    o donner le centre d'un sprite.
    o sauver un sprite.
    o consulter un ensemble de sprite.
   Il contient sur la droite un ensemble de fleches bleues, une
   rouge et un texte.
   La double fleche >> permet de creer un nouveau sprite.
   Les fleches bleues < et > permettent de se deplacer parmis
   les sprites. Et, la fleche rouge permet la sauvegarde d'un
   sprite. Le texte a droite indique le numero du sprite courant
   et le nombre de sprite crees.
 o Le bas de l'ecran 320*200 contient l'image ou l'on peut
   capturer les sprites.

Capturer un sprite :
 o D'abord un clic sur GET dans le menu.
 o Ensuite, un clic sur un des coins du sprite et on garde le doigt
   appuye jusqu'au 2eme coin du sprite.
   Un sprite ne doit pas faire plus de 115 pixels de large (par
   contre la hauteur peut etre quelconque)
   Un sprite ne doit pas etre entierement noir (sinon plantage).
   C'est pas la peine de viser les bords au pixel pres car le
   noir n'est pas code en memoire. Le sprite prendra toujours la
   meme taille et le meme temps a afficher.

Centrer un sprite :
  Les touches de fonctions F3 a F10 permettent de centrer le sprite.
  La mire est graduee par pas de 10 pixels, elle permet de
  positionner precisiment le sprite.
  Un clic finira le centrage.

Le selecteur de fichier :
Deja, il ne faut pas toucher a la souris pendant l'operation.
Le principe est une entree au clavier avec completion automatique.
Quand on rentre un chemin (ex f:\tm ), un appui sur TAB va completer
l'entree au maximun et afficher la liste de tous les fichiers qui
pourront correspondre. Les repertoires sont affiches en rouge et
les fichiers normaux en jaune.
ESC permet d'abandonner la selection, l'operation est annulee.
Pour ecrire \, il faut taper / (oui c'est zarbi)

Utilisation des sprites :
Une fois en memoire (avec un incbin en asm, ou charger depuis le
disque), il suffit d'appeler la fonction SPRITE_afficher()
(voir sources ASM et C de l'editeur)
