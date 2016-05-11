Camus Tristan
Cojez Arnaud


Tout a été fait et fonctionne parfaitement

E6 :
   Nous avons fait en sorte que la rotation se fasse autour des axes y et z du modèle.
   Ce qui fait qu'on a plus l'impression de "conduire" le modèle de l'intérieur que de l'orienter de l'extérieur, mais ce choix est motivé par le fait que le comportement de l'orientation est constant et plus intuitif quelque soit l'orientation du modèle
   par exemple : on sait que quand on tourne à gauche, la vache ira sur SA gauche, etc.

   Pour replacer le modèle de la vache en face de nous après une rotation "aléatoire", nous pouvons d'abord la replacer par rapport aux axes y et z. Ensuite, en faisant des petits cercles avec la souris nous arrivons à la replacer par rapport au dernier axe. (snapshots q6_4, 5, 6)
   
   On pourrait faire en sorte de choisir les 2 axes sur lesquels on agit, ou alors modifier un axe quand on appuie sur une touche, etc.

Notes :
- Contrairement à ce qui est noté sur le schéma, pickingray n'est pas 
dirigé vers P mais vers P' (ex5 q3)
- Le lien pour le wiki de blender est mort (ex 6)

- Dans GLTool.h et Camera.h, cmath n'était pas inclus, par conséquent 
les appels aux fonctions tan et fabs provoquaient des erreurs à la compilation. Nous avons donc inclus cette bibliothèque pour régler le problème.
