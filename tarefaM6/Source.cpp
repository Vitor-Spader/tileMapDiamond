/* 
 *
 * Adaptado por Rossana Baptista Queiroz
 * Base: https://learnopengl.com/#!Getting-started/Hello-Triangle 
 * para as disciplinas de Processamento Gráfico/Computação Gráfica/Fundamentos de CG - Unisinos
 * Versão inicial: 07/04/2017
 * Última atualização em 06/07/2024
 *
 */

#include <iostream>
#include <string>
#include <assert.h>
#include <windows.h>  
#include <filesystem>

using namespace std;

// DEPENDENCIES
// GLAD
#include <glad/glad.h>
// GLFW
#include <GLFW/glfw3.h>
// GLM
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// STB_IMAGE
#include <stb_image.h>

// COMMON - Códigos comuns entre os projetos
// Classe que gerencia os shaders
#include "Shader.h"

// APP DOMAIN - Códigos específicos desta aplicação
#include "Sprite.h"
#include "Tile.h"

// Protótipos das funções de callback 
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void resize_callback(GLFWwindow* window, int width, int height);

// Protótipos das funções
GLuint loadTexture(string filePath, int &imgWidth, int &imgHeight);
bool checkCollision(Sprite &one, Sprite &two);
int startEndGame();
glm::vec3 computePosOnMap(glm::vec2 iPos, glm::vec2 posIni, glm::vec2 tileSize);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 1920, HEIGHT = 1080;
glm::vec2 viewportSize;
bool resize = false;

//Variáveis globais
int dir = Sprite::directions::NONE; //controle da direção do personagem
bool isEnableCoin = true, isEnableCoin2 = true;
int lavaTile = 3;//Lava

//Variáveis para armazenar as infos do tileset
GLuint tilesetTexID;
glm::vec2 offsetTex; //armazena o deslocamento necessário das coordenadas de textura no tileset
GLuint VAOTile;
int nTiles;
glm::vec2 tileSize;

//Variáveis para armazenar as infos do tilemap
glm::vec2 tilemapSize;
const int MAX_COLUNAS = 15;
const int MAX_LINHAS = 15;
int tilemap[MAX_LINHAS][MAX_COLUNAS]; //este é o mapa de índices para os tiles do tileset
glm::vec2 posIni; //pode virar parâmetro 

//funcao para controle do fluxo do game
bool startGame();

//Função para fazer a leitura do tilemap do arquivo
void loadMap(string fileName);
void drawDiamondMap(Tile &tile);

//Deixando shader de debug global para facilitar acesso nas funções 
Shader *shaderDebug;
Shader *shader;

void generatePositionCoin(float &posx, float &posy);
glm::vec2 playeriPos, coiniPos; //posição do indice do personagem no mapa
// Função MAIN
int main()
{
	int tecla = 10;//numero da tecla enter
	while(tecla == 10)
	{
		if(startGame()){
			cout << "Parabens você venceu o jogo" << endl;
		}
		else{
			cout << "Você perdeu o jogo" << endl;
		}
	
		cout << "Deseja jogar novamente? tecle Enter, tecle outra tecla" << endl;
		tecla = cin.get();
	}
}

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - é chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	switch (key)
	{
	case GLFW_KEY_KP_4:
		dir = Sprite::directions::WEST;
		playeriPos.y += 0.25;
		break;
	case GLFW_KEY_KP_6:
		dir = Sprite::directions::EAST;
		playeriPos.y -= 0.25;
		break;
	case GLFW_KEY_KP_8:
		dir = Sprite::directions::NORTH;
		playeriPos.x -= 0.25;
		break;
	case GLFW_KEY_KP_2:
		dir = Sprite::directions::SOUTH;
		playeriPos.x += 0.25;
		break;
	case GLFW_KEY_KP_9:
		dir = Sprite::directions::NORTHEAST;
		playeriPos.y -= 0.25;
		playeriPos.x -= 0.25;
		break;
	case GLFW_KEY_KP_3:
		dir = Sprite::directions::SOUTHEAST;
		playeriPos.y -= 0.25;
		playeriPos.x += 0.25;
		break;
	case GLFW_KEY_KP_7:
		dir = Sprite::directions::NORTHWEST;
		playeriPos.y += 0.25;
		playeriPos.x -= 0.25;
		break;
	case GLFW_KEY_KP_1:
		dir = Sprite::directions::SOUTHWEST;
		playeriPos.y += 0.25;
		playeriPos.x += 0.25;
		break;
	default:
		break;
	}

	if (action == GLFW_RELEASE)
	{
		dir = Sprite::directions::NONE;
	}

}

GLuint loadTexture(string filePath, int &imgWidth, int &imgHeight)
{
	GLuint texID;

	// Gera o identificador da textura na memória 
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
    	if (nrChannels == 3) //jpg, bmp
    	{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    	}
    	else //png
    	{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    	}
    	glGenerateMipmap(GL_TEXTURE_2D);

		imgWidth = width;
		imgHeight = height;

		stbi_image_free(data);

		glBindTexture(GL_TEXTURE_2D, 0);

	}
	else
	{
    	 std::cout << "Failed to load texture " << filePath << std::endl;
	}
	return texID;
}

bool checkCollision(Sprite &one, Sprite &two)
{
	one.getAABB();
	two.getAABB();
    // collision x-axis?
    bool collisionX = one.getPMax().x >= two.getPMin().x &&
        two.getPMax().x >= one.getPMin().x;
    // collision y-axis?
    bool collisionY = one.getPMax().y >= two.getPMin().y &&
        two.getPMax().y >= one.getPMin().y;
    // collision only if on both axes
    return collisionX && collisionY;
}

//Função para fazer a leitura do tilemap do arquivo
void loadMap(string fileName)
{
	ifstream arqEntrada;
	arqEntrada.open(fileName); //abertura do arquivo
	if (arqEntrada)
	{
		///leitura dos dados
		string textureName;
		int width, height;
		//Leitura das informações sobre o tileset
		arqEntrada >> textureName >> nTiles >> tileSize.y >> tileSize.x;
		tilesetTexID = loadTexture("C:\\Users\\vitor\\Desktop\\Programacao\\TarefaM6\\tarefaM6\\tex\\" + textureName, width, height);
		//Só pra debug, printar os dados
		//cout << textureName << " " << nTiles << " " << tileSize.y << " " << tileSize.x << endl;
		//Leitura das informações sobre o mapa (tilemap)
		arqEntrada >> tilemapSize.y >> tilemapSize.x; //nro de linhas e de colunas do mapa
		//cout << tilemapSize.y << " " << tilemapSize.x << endl;
		for (int i = 0; i < tilemapSize.y; i++) //percorrendo as linhas do mapa
		{
			for (int j = 0; j < tilemapSize.x; j++) //percorrendo as colunas do mapa
			{
				arqEntrada >> tilemap[i][j];
			}
		}
	
	}
	else
	{
		cout << "Houve um problema na leitura de " << fileName << endl;
	}
}

void drawDiamondMap(Tile &tile)
{
	int index_sprite = 0;//inicia no zero sempre

	for (int i=0; i < tilemapSize.y; i++)
	{
		for (int j=0; j < tilemapSize.x; j++)
		{
			int indiceTile = tilemap[i][j];
			tile.desenharNaPos(i, j, indiceTile, posIni,DIAMOND);
		}	
	}
}

void resize_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	viewportSize.x = width;
	viewportSize.y = height;

	posIni.x = viewportSize.x/2.0;
	posIni.y = 0.0;
	resize = true;	
}

glm::vec3 computePosOnMap(glm::vec2 iPos, glm::vec2 posIni, glm::vec2 tileSize) //considerando DIAMOND
{
	glm::vec3 pos;
	//Encontra a posição no mapa para os índices ij
	pos.x = posIni.x + (iPos.x-iPos.y) * tileSize.x/2.0f;
	pos.y = posIni.y + (iPos.x+iPos.y) * tileSize.y/2.0f;
	return pos;
}

bool startGame()
{

	//inicializa variaveis globais para o valor padrão
	isEnableCoin = isEnableCoin2 = true;

		// Inicialização da GLFW
	glfwInit();

	// Muita atenção aqui: alguns ambientes não aceitam essas configurações
	// Você deve adaptar para a versão do OpenGL suportada por sua placa
	// Sugestão: comente essas linhas de código para descobrir a versão e
	// depois atualize (por exemplo: 4.5 com 4 e 5)
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/


	// Criação da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "TileMap Diamond", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);
	glfwSetWindowSizeCallback(window, resize_callback);

	// GLAD: carrega todos os ponteiros de funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Obtendo as informações de versão
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	viewportSize.x = WIDTH;
	viewportSize.y = HEIGHT;
	glViewport(0, 0, viewportSize.x, viewportSize.y);

	//Habilitando a transparência
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Habilitando o teste de profundidade
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);

	// Compilando e buildando o programa de shader
	shader = new Shader("C:\\Users\\vitor\\Desktop\\Programacao\\TarefaM6\\tarefaM6\\HelloTriangle.vs","C:\\Users\\vitor\\Desktop\\Programacao\\TarefaM6\\tarefaM6\\HelloTriangle.fs");
	shaderDebug = new Shader("C:\\Users\\vitor\\Desktop\\Programacao\\TarefaM6\\tarefaM6\\HelloTriangle.vs","C:\\Users\\vitor\\Desktop\\Programacao\\TarefaM6\\tarefaM6\\HelloTriangleDebug.fs");

	//Leitura do tilemap
	loadMap("C:\\Users\\vitor\\Desktop\\Programacao\\TarefaM6\\tarefaM6\\maps\\map2.txt");
	posIni.x = viewportSize.x/2.0;
	posIni.y = tileSize.y/2.0;

	//Criação de um objeto Tile
	Tile tile;
	tile.inicializar(tilesetTexID, 1, nTiles, glm::vec3(400.0,-200.0,0.0), glm::vec3(tileSize.x,tileSize.y,1.0),0.0,glm::vec3(1.0,1.0,1.0));
	tile.setShader(shader);
	tile.setShaderDebug(shaderDebug);

	//Criação de um objeto Sprite para o personagem
	//Carregando as texturas e armazenando seu ID
	int imgWidth, imgHeight;
	GLuint texID = loadTexture("C:\\Users\\vitor\\Desktop\\Programacao\\TarefaM6\\tarefaM6\\tex\\char.png", imgWidth, imgHeight);
	
	playeriPos.x = 0; //coluna
	playeriPos.y = 0; //linha
	glm::vec3 playerPos = computePosOnMap(playeriPos, posIni, tileSize);

	Sprite player, coin, coin2;
	player.inicializar(texID, 4, 6, playerPos, glm::vec3(imgWidth*3,imgHeight*3,1.0),0.0,glm::vec3(1.0,0.0,1.0));
	player.setShader(shader);
	player.setShaderDebug(shaderDebug);
	
	texID = loadTexture("C:\\Users\\vitor\\Desktop\\Programacao\\TarefaM6\\tarefaM6\\tex\\Gold_1.png", imgWidth, imgHeight);

	coiniPos.x = 3;
	coiniPos.y = 3;
	glm::vec3 coinPos = computePosOnMap(coiniPos, posIni, tileSize);

	coin.inicializar(texID, 1, 1, coinPos, glm::vec3(imgWidth*0.09,imgHeight*0.09,1.0),0.0,glm::vec3(1.0,0.0,1.0));
	coin.setShader(shader);
	coin.setShaderDebug(shaderDebug);

	coiniPos.x = 13;
	coiniPos.y = 13;
	coinPos = computePosOnMap(coiniPos, posIni, tileSize);

	coin2.inicializar(texID, 1, 1, coinPos, glm::vec3(imgWidth*0.09,imgHeight*0.09,1.0),0.0,glm::vec3(1.0,0.0,1.0));
	coin2.setShader(shader);
	coin2.setShaderDebug(shaderDebug);
	//Habilita o shader que será usado (glUseProgram)
	(*shader).Use();
	glm::mat4 projection = glm::ortho(0.0, (double) viewportSize.x,(double) viewportSize.y, 0.0, -1.0, 1.0);
	//Enviando para o shader via variável do tipo uniform (glUniform....)
	(*shader).setMat4("projection",glm::value_ptr(projection));

	glActiveTexture(GL_TEXTURE0); //Especificando que o shader usará apenas 1 buffer de tex
	(*shader).setInt("texBuffer", 0); //Enviando para o shader o ID e nome da var que será o sampler2D 

	//Habilita o shader de debug
	//shaderDebug->Use();
	//shaderDebug->setMat4("projection",glm::value_ptr(projection));

	int lastx = -1, lasty = -1;
	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		if (resize) //se houve alteração no tamanho da janela
		{
			//Atualizamos a matriz de projeção ortográfica para ficar com relação 1:1 mundo e tela
			(*shader).Use();
			glm::mat4 projection = glm::ortho(0.0, (double) viewportSize.x*1.5,(double) viewportSize.y*1.5, 0.0, -1.0, 1.0);
			//Enviando para o shader via variável do tipo uniform (glUniform....)
			(*shader).setMat4("projection",glm::value_ptr(projection));

			//shaderDebug->Use();
			//shaderDebug->setMat4("projection",glm::value_ptr(projection));

			posIni.x = viewportSize.x/2.0;
			posIni.y = tileSize.y/2.0;
			resize = false; //bem importante isso!!
		}

		// Atualizando input personagem
		// Verifica flags para movimentação do personagem
		if (dir != Sprite::directions::NONE)
		{	
			player.setPos(computePosOnMap(playeriPos, posIni, tileSize));
			player.desenhar(true);
			if (checkCollision(player, coin)){
				coin.removeSprite();
				isEnableCoin = false;
			}
			else if (checkCollision(player, coin2)){
				coin2.removeSprite();
				isEnableCoin2 = false;
			}
			if (tilemap[(int)round(playeriPos.y)][(int)round(playeriPos.x)] == 3)
			{
				glfwSetWindowShouldClose(window, GL_TRUE);
				glfwTerminate();
				return false;
			}
			else{
				if (lastx >= 0 && lasty >= 0)
					tilemap[lasty][lastx] = 3;
				lasty = (int)round(playeriPos.y);
				lastx = (int)round(playeriPos.x);
				tilemap[(int)round(playeriPos.y)][(int)round(playeriPos.x)] = 2;
			}
				

		}

		// Limpa o buffer de cor
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// tile.desenhar();
		drawDiamondMap(tile);
		player.desenhar(false);

		if (!isEnableCoin && !isEnableCoin2){
			cout<<"parabens voce venceu o game"<<endl;
			break;
		}
		if (isEnableCoin)
			coin.desenhar(false);

		if (isEnableCoin2)
			coin2.desenhar(false);
		
		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	
	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return true;
}