# TP2 : Évaluation d'un masque de segmentation

## Objectif
Dans ce TP, vous allez évaluer un masque de segmentation produit au TP précédent.

L'objectif est double. Vous devez d'abord comparer une prédiction à une vérité terrain avec plusieurs métriques simples. Vous devez ensuite observer qu'une partie de cette évaluation, le calcul exact du diamètre maximal d'une composante connexe, devient coûteuse et se prête bien à une accélération avec CUDA.

## Données d'entrée
Le programme prend trois fichiers en entrée :
- un masque prédit recadré produit par le TP précédent ;
- un fichier texte contenant les informations de recadrage ;
- un masque de vérité terrain pleine taille.

Exemple avec les fichiers du projet :

```bash
.\tp2.exe ..\tp1-bis\mask_final.png ..\tp1-bis\mask_final_crop.txt ..\data\GT\pointCloud_data_2026-02-19_12-03-32_rgb_semantic_mask.png
```

Le fichier de recadrage contient les champs `originalWidth`, `originalHeight`, `cropX`, `cropY`, `cropW` et `cropH`.

## Travail demandé
Vous devez compléter les fonctions dans `student_functions.cpp` et `student_functions.cu`.

Le plus simple est d'avancer dans l'ordre suivant.

### 1. Replacer le masque dans l'image complète
Commencez par `reconstructFullMask`. Cette fonction doit reconstruire un masque pleine taille à partir du masque recadré et replacer correctement la zone utile dans une image de dimensions `originalWidth x originalHeight`.

La lecture du fichier de recadrage est déjà fournie par `loadCropInfo`.

### 2. Calculer les métriques globales
Une fois le masque reconstruit, calculez les métriques les plus simples :
- `countOverlap` pour compter les pixels vrais dans les deux masques ;
- `countUnion` pour compter les pixels vrais dans au moins un des deux masques ;
- `computeSimpleScore` pour calculer `overlap / total_pixels` ;
- `computeIoU` pour calculer `overlap / union`.

Pour l'IoU, on adopte la convention suivante : si `union == 0`, alors `IoU = 1.0`.

### 3. Analyser la structure du masque
Vous étudierez ensuite la structure des deux masques. Il faut d'abord compter le nombre de composantes connexes avec `countConnectedComponents`, en utilisant une connectivité 8.

Pour calculer le diamètre, vous devrez aussi extraire les pixels d'une composante connexe avec `extractComponentPixels`. À partir d'un pixel blanc non visité, cette fonction doit récupérer tous les pixels de la composante et stocker leurs coordonnées `(x, y)`.

### 4. Accélérer ce calcul avec CUDA
La dernière étape consiste à compléter la partie CUDA dans `student_functions.cu`, ainsi que la version C++ de référence dans `student_functions.cpp`.

L'extraction des composantes reste sur le CPU. En revanche, la partie la plus coûteuse, la comparaison brute force de toutes les paires de pixels d'une composante, doit être accélérée avec CUDA. Dans le starter actuel, le diamètre C++ et le diamètre CUDA sont tous les deux à compléter. L'idée côté CUDA est la suivante :
- un thread CUDA traite une paire de pixels ;
- il calcule une distance au carré ;
- il essaye de mettre à jour le maximum avec `atomicMax`.

Concrètement, dans `student_functions.cu`, il faut :
- compléter le kernel `kernelComponentDiameter` ;
- calculer `totalPairs`, `blockSize` et `gridSize` ;
- lancer correctement le kernel ;
- conserver `cudaComputeLargestComponentDiameter` comme parcours global des composantes.

## Ce que le programme doit afficher
Le programme doit afficher :
- les dimensions des deux masques comparés ;
- `overlap` ;
- `union` ;
- le nombre de composantes connexes dans la prédiction ;
- le nombre de composantes connexes dans la vérité terrain ;
- la différence du nombre de composantes connexes ;
- le diamètre maximal de la prédiction en C++ ;
- le diamètre maximal de la vérité terrain en C++ ;
- le diamètre maximal de la prédiction en CUDA ;
- le diamètre maximal de la vérité terrain en CUDA ;
- la différence de diamètre maximal ;
- l'IoU ;
- le score simple ;
- le temps du diamètre en C++ ;
- le temps du diamètre en CUDA ;
- le temps total des métriques.

L'objectif n'est pas seulement d'obtenir les bonnes valeurs, mais aussi d'observer le gain de temps apporté par CUDA sur le calcul du diamètre.

## Compilation
Ce TP n'utilise pas OpenCV.

Dans l'environnement de la salle, la compilation CUDA fonctionne avec `nvcc` et un chemin explicite vers `cl.exe` :

```bash
nvcc -ccbin "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.29.30133\bin\Hostx64\x64\cl.exe" -std=c++17 -O2 -o tp2.exe tp2.cpp student_functions.cpp student_functions.cu ../io.cpp
```

A l'IUT, remplacer par
```bash
nvcc -ccbin "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Tools\MSVC\14.29.30133\bin\Hostx64\x64" -std=c++17 -O2 -o tp2.exe tp2.cpp student_functions.cpp student_functions.cu ../io.cpp
```

## Exécution
Depuis le dossier `tp2` :

```bash
.\tp2.exe ..\tp1-bis\mask_final.png ..\tp1-bis\mask_final_crop.txt ..\data\GT\pointCloud_data_2026-02-19_12-03-32_rgb_semantic_mask.png
```

## Remarque sur les fichiers
Le masque prédit utilisé ici est celui produit par `TP1-bis`. Dans l'état actuel du projet, ce fichier s'appelle `mask_final.png`. Les masques de vérité terrain du dossier `data/GT` sont fournis en PNG.

## Vérifications attendues
- les dimensions du masque reconstruit doivent correspondre à celles de la vérité terrain ;
- les valeurs de `overlap`, `union` et `IoU` doivent être cohérentes ;
- le nombre de composantes connexes doit être plausible ;
- le diamètre maximal en CUDA doit être identique au diamètre maximal en C++ pour un même masque ;
- le temps du diamètre en CUDA doit être plus faible que le temps du diamètre en C++ sur une image réelle.

## Fichiers
- `tp2.cpp` : programme principal ;
- `student_functions.h` : déclarations ;
- `student_functions.cpp` : fonctions CPU ;
- `student_functions.cu` : calcul du diamètre avec CUDA.

## Durée
2h
