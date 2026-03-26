# TP1 : Manipulation d'une carte de profondeur

## Objectif
Manipuler une carte de profondeur en travaillant trois niveaux d'implementation :
- une fonction CPU en C++
- un kernel CUDA
- un wrapper CUDA cote hote

Le but est de comprendre comment passer d'un traitement sequentiel simple a une execution GPU, sans refaire trois fois exactement le meme exercice.

## Repartition du travail
Dans ce TP, vous devez completer exactement trois elements.

### 1. Normalisation en CPU : `normalizeDepth`
Dans `student_functions.cpp`, completez la fonction `normalizeDepth`.

Soit \( d \) la carte de profondeur originale, avec \( d_{\min} = \min(d) \) et \( d_{\max} = \max(d) \). La normalisation lineaire vers \([0, M]\) est donnee par :

\[
d'[i,j] = \frac{d[i,j] - d_{\min}}{d_{\max} - d_{\min}} \times M
\]

Ici, on prendra \( M = 65535 \) pour preserver la precision des cartes de profondeur 16 bits.

### 2. Seuillage dans un kernel CUDA : `kernelThreshold`
Dans `student_functions.cu`, completez le kernel `kernelThreshold`.

Le seuillage binaire avec seuil \( t \) cree un masque 8 bits :

\[
d'[i,j] = 255 \cdot \mathbb{I}(d[i,j] > t)
\]

ou \( \mathbb{I} \) vaut 1 si la condition est vraie, sinon 0.

### 3. Wrapper CUDA pour l'inversion : `cudaInvertDepth`
Dans `student_functions.cu`, completez la fonction `cudaInvertDepth`.

L'inversion est definie par :

\[
d'[i,j] = 65535 - d[i,j]
\]

Ici, le kernel d'inversion est deja fourni. Votre travail consiste a gerer la partie hote :
- allocation GPU
- copie Host -> Device
- configuration de la grille et des blocs
- lancement du kernel
- copie Device -> Host
- liberation de la memoire GPU

## Ce qui est deja fourni
Les elements suivants sont deja implementes et servent d'exemples :
- `invertDepth` en CPU
- `thresholdDepth` en CPU
- `kernelNormalize`
- `cudaNormalizeDepth`
- `cudaThresholdDepth`

Servez-vous de ces fonctions comme modeles pour completer les parties demandees.

## Conseils
- Commencez par la version CPU `normalizeDepth`.
- Passez ensuite au kernel `kernelThreshold`.
- Terminez par le wrapper `cudaInvertDepth`.
- Testez regulierement au lieu d'attendre la fin.

## Compilation
```bash
nvcc -o tp1 tp1.cpp student_functions.cu ../io.cpp ../utils_cuda.h -I..
```

## Execution
```bash
./tp1 depth_image.bin [mode]
```

Modes :
- `cpu` : execute seulement les versions CPU
- `cuda` : execute seulement les versions CUDA
- `both` : execute les deux versions

## Verifications attendues
- `normalizeDepth` doit utiliser toute la plage `[0, 65535]`.
- `kernelThreshold` doit fonctionner meme si les dimensions de l'image ne sont pas des multiples de `16`.
- `cudaInvertDepth` doit bien modifier les donnees sur le GPU puis recopier le resultat sur le CPU.
- Les resultats CUDA doivent etre coherents avec les versions CPU de reference.
- Comparez les temps d'execution CPU et CUDA affiches par le programme, puis discutez brievement si l'acceleration observee vous parait importante ou non.

## Fichiers
- `tp1.cpp` : programme principal
- `student_functions.h` : declarations
- `student_functions.cpp` : fonctions CPU
- `student_functions.cu` : kernels et wrappers CUDA

## Duree
2h
