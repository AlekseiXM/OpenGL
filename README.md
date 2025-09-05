# Sistema Solar 3D em OpenGL

Simulação interativa do **Sistema Solar** em 3D usando **C++** e **OpenGL/GLUT**.  
O projeto apresenta planetas com órbitas, lua da Terra, anéis de Saturno, estrelas de fundo e seleção de planetas com efeito de blink.

---

## 💻 Funcionalidades

- Estrelas de fundo em 3D.
- Planetas com cores e tamanhos aproximados.
- Lua orbitando a Terra.
- Anéis de Saturno.
- Seleção de planetas com caixa de seleção piscante.
- Zoom controlado pelas teclas **Z** (aproximar) e **X** (afastar).
- Informações do planeta selecionado exibidas na tela.

---

## 🖼️ Imagens (Exemplo)

Simulação do sistema solar
<img width="1584" height="830" alt="image" src="https://github.com/user-attachments/assets/46161778-f810-4fd2-b308-4a0a2f379cf8" />

Seleção de planeta
<img width="1597" height="834" alt="image" src="https://github.com/user-attachments/assets/28c9b6e6-cdb1-4b62-9bcc-d3384ea0d3b4" />


---

## 🛠️ Requisitos

- Compilador C++17 ou superior  
- OpenGL  
- GLUT ou FreeGLUT  

---

## 📦 Como Compilar

### Linux / macOS:

```bash
g++ main.cpp -o SistemaSolar -lGL -lGLU -lglut
./SistemaSolar
```

### Windows (MinGW)

```bash
g++ main.cpp -o SistemaSolar.exe -lfreeglut -lopengl32 -lglu32
SistemaSolar.exe
```
### Controles

| Tecla / Ação    | Função                             |
| --------------- | ---------------------------------- |
| Setas ↑ ↓ ← →   | Rotaciona a câmera ao redor do Sol |
| Z               | Aproxima a câmera (Zoom in)        |
| X               | Afasta a câmera (Zoom out)         |
| Botão direito do mouse | Seleciona o planeta     |

## 📝 Autor
Aleksei Xavier – https://github.com/AlekseiXM

## 📄 Licença
Este projeto é aberto e livre para estudo e modificação.
