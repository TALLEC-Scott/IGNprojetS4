# Topography IGN Project

### Description
The goal of this project is to convert a topographic map into an explorable 3D model. This involves processing the map to extract topographic lines, assigning relief, modeling the map, implementing a free-moving camera, and creating a graphical interface and website.

### Features
- **Topographic Map Processing:** Extracts and reconstructs topographic lines.
- **Relief Assignment:** Labels and assigns altitudes to map areas.
- **3D Modeling:** Converts map data into a 3D model using OpenGL.
- **Free Camera Movement:** Allows navigation through the 3D model.
- **Graphical Interface:** User-friendly interface for loading maps, viewing analysis, and interacting with the model.
- **Website:** Presents the project and its development process.

### Installation
1. **Clone the repository:**
   ```bash
   git clone https://github.com/your-username/IGNprojetS4.git
   ```
2. **Navigate to the project directory:**
   ```bash
   cd IGNprojetS4
   ```
3. **Compile the project:**
   ```bash
   make
   ```

### Usage
1. **Run the executable:**
   ```bash
   ./223D
   ```
2. **Ensure `app.glade` is in the `Application` folder for GTK objects.**

### Requirements
- **Libraries:**
  - C standard libraries
  - GTK for the graphical interface
  - SDL2 for topographic map processing
  - OpenGL for 3D rendering

### Contributions
1. **Fork the repository.**
2. **Create a new branch:**
   ```bash
   git checkout -b feature-branch
   ```
3. **Make your changes and commit:**
   ```bash
   git commit -m 'Add feature'
   ```
4. **Push to the branch:**
   ```bash
   git push origin feature-branch
   ```
5. **Submit a pull request.**

### License
This project is licensed under the MIT License - see the LICENSE file for details.

### Authors
- [Scott Tallec](https://github.com/TALLEC-Scott)
- [Vincent Libeskind](https://github.com/VincentKobz)
- [Yann Boudry](https://github.com/yannboudry)
- [Azeline Aillet](https://github.com/Azeline)
- [Pierre-Corentin Auger](https://github.com/PierreCorentin)

### Screenshots


2D Elevation Extrapolation
![map_ign_3 (1)](https://github.com/TALLEC-Scott/IGNprojetS4/assets/63871153/4258887c-c66d-40fc-bf16-671473e31362)
![elevation (1)](https://github.com/TALLEC-Scott/IGNprojetS4/assets/63871153/13d3b941-3edd-4033-b6ae-6054298fda87)

3D Model View
![points](https://github.com/TALLEC-Scott/IGNprojetS4/assets/63871153/05c306c6-6354-4f7b-a4fe-c07e6dcc2b25)
![lines](https://github.com/TALLEC-Scott/IGNprojetS4/assets/63871153/5b0a4176-1011-4903-8a85-d9ee732a6d5c)
![standard](https://github.com/TALLEC-Scott/IGNprojetS4/assets/63871153/c0237c7c-4ba1-4a03-846e-46dc3ef8478b)

Main Interface
![main_screen_start](https://github.com/TALLEC-Scott/IGNprojetS4/assets/63871153/1dd73b39-8150-46f6-b104-0d9b66159d16)
![main_screen_end](https://github.com/TALLEC-Scott/IGNprojetS4/assets/63871153/decfb1fd-1ec9-4fe0-87b5-528eb5b6b293)
