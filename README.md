## Visor NIfTI - Aplicación en C++ con OpenCV e ITK

### Descripción
Esta es una aplicación de escritorio en C++ que permite visualizar imágenes médicas en formato NIfTI (.nii). Utiliza las bibliotecas OpenCV para el procesamiento de imágenes y ITK para el manejo de archivos NIfTI. La aplicación está diseñada para permitir la visualización de cortes (slices) de imágenes de resonancia magnética (RM), con la capacidad de aplicar filtros y realizar operaciones básicas sobre la imagen.

### Características

- [x] **Visualización interactiva de imágenes NIfTI.**
- [x] **Navegación entre diferentes cortes (slices) de la imagen.**
- [x] **Filtros aplicables como ecualización, suavizado y detección de bordes.**
- [x] **Resaltado de áreas de interés, como tumores, utilizando técnicas de segmentación.**
- [x] **Generación de video a partir de los cortes con transiciones suaves.**
- [x] **Interfaz gráfica creada con Qt.**


### Requisitos
- C++11 o superior
- CMake para la configuración del proyecto.
- OpenCV (4.5.0 o superior)
- ITK (5.2 o superior)
- Qt (5.15 o superior)
- Compilador C++ compatible (como GCC o Clang)

### Instalación
Clona el repositorio:
```text
git clone https://github.com/Abenavidese/VisionProyecto.git
```

Accede al directorio del proyecto:
```text
cd VisionProyecto
```

Crea un directorio para la compilación:
```text
mkdir build
cd build
```
Ejecuta CMake para generar los archivos de construcción:

```text
cmake ..
```

Compila el proyecto:
```text
make
```

Ejecuta la aplicación:
```text
./visor_nifti
```
## Uso

- Abre la aplicación y carga un archivo NIfTI (.nii).
- Usa los controles deslizantes para navegar entre los diferentes cortes de la imagen.
- Aplica filtros como suavizado, detección de bordes o ecualización para mejorar la visualización.
- Resalta áreas de interés, como tumores, utilizando las herramientas de segmentación disponibles.
- Puedes generar un video de los cortes con transiciones suaves.

## Estructura del proyecto

```text
visor-nifti/
│
├── CMakeLists.txt        # Archivo de configuración de CMake
├── src/                  # Código fuente de la aplicación
│   ├── main.cpp          # Punto de entrada de la aplicación
│   ├── nifti_loader.cpp  # Funciones para cargar imágenes NIfTI
│   ├── image_processor.cpp  # Funciones para procesar imágenes
│   └── ...
├── include/              # Archivos de cabecera
│   ├── nifti_loader.h    # Definiciones de las funciones de carga
│   ├── image_processor.h  # Definiciones de funciones de procesamiento
│   └── ...
├── output/               # Archivos de recursos (imágenes, iconos, etc.)
├── README.md             # Este archivo
└── ...
```

## AUTORES

- ANTHONY ALEXANDER BENAVIDES ERIQUE
- BRYAM JESUS PERALTA NAVARRO
















