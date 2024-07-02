# IGNprojetS4

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

### Acknowledgments
- Scott Tallec
- Yann Boudry
- Vincent Libeskind
- Azeline Aillet
- Pierre-Corentin Auger
