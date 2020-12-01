#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <map>
#include <vector>
#include <unordered_map>
#include <list>

// Node class implementation - Graph
class Node {
public:
	std::string resource;
	bool usable;
	Node(std::string, bool);
	~Node();
};

Node::Node(std::string resource, bool state) {
	this->resource = resource;
	this->usable = state;
}

Node::~Node() {
}

void print_prompt() {
	std::cout << "rts > ";
}

void print_commands() {
	std::cout << "~ ResourceManagerRTS version 0.1" << std::endl;
	std::cout << "	Enter \".help\" for commands" << std::endl;
	std::cout << "	Use \".q\" to quit program" << std::endl;
	std::cout << "	Use \".delete 'NodeName'\" to delete the node from the graph." << std::endl;
	std::cout << "	Use \".insert 'StartNodeName' 'EndNodeName'\" to insert a new link in the graph." << std::endl;
	std::cout << "	Use \".display\" to show the graph adjacency list representation and the usability list of nodes." << std::endl;
	std::cout << "~" << std::endl;
	std::cout << std::endl;
}

//The printListOfVertices() function will print the list that contains the vertices of the graph and their state, useable or unuseable.
//Pre: List must contain the vertices of the graph.
//Post: Output message showing every vertice of the graph and its usuability in the screen.
void printListOfVertices(std::vector<Node> listOfVertices) {
	std::vector<Node>::iterator it;
	std::cout << "~	List of vertices usability: " << std::endl;
	std::cout << std::endl;
	for (it = listOfVertices.begin(); it < listOfVertices.end(); it++) { // Verifies each Node and print its usable state.
		std::cout << "Vertex: [" << (*it).resource << "]";
		std::cout << "  ";
		if ((*it).usable) {
			std::cout << "-> usable" << std::endl;
		}
		else {
			std::cout << "-> unusable" << std::endl;
		}
	}
}

//The deleteVertex() function will erase from the graph the input vertex.
//Pre: None.
//Post: An updated graph with the vertex already deleted.
void deleteVertex(std::unordered_map<std::string, std::list<std::string>> *adjacencyList, std::vector<Node> *vertices, std::string vertex) {
	//Delete the given vertex from the graph.
	bool found = false;

	//std::cout << "Node:				Links: " << std::endl;

	for (auto &value : *adjacencyList) {
		std::string node = value.first; // value.first contains the node of the graph.
		std::list<std::string> adjacentVertices = value.second; // value.second contains the links of the vertex.
		std::list<std::string>::iterator itr = value.second.begin();

		while (itr != value.second.end()) {
			if ((*itr) == vertex) { //Verifies if the input vertex is in the adjacentVertices to be able to delete the link.
				found = true;
				value.second.erase(itr); // Delete link from the vertex.
				//Update the state of the vertex to unusable since the node it relies on got deleted from the adjacentVertices.
				for (std::vector<Node>::iterator it = (*vertices).begin(); it < (*vertices).end(); it++) {
					if ((*it).resource == node) {
						(*it).usable = false;
					}
				}
				break;
			}
			++itr;
		}
	}

	for (auto &value : *adjacencyList) { // Verifies if the input vertex is in the unordered map as a Node.
		std::string node = value.first; // value.first contains the node of the graph.

		if (node == vertex) {
			adjacencyList->erase(node);
			//Erase the input vertex from the graph.
			for (std::vector<Node>::iterator it = (*vertices).begin(); it < (*vertices).end(); it++) {
				if ((*it).resource == vertex) {
					vertices->erase(it); // Erase the input vertex from the vector which contains each Node.
					break;
				}
			}
			found = true;
			break;
		}
	}

	if (!found) {
		std::cout << "Node [" << vertex << "] is not a node part of the graph." << std::endl;
		std::cout << std::endl;
	}
	else if (found) {
		std::cout << "Node [" << vertex << "] deleted." << std::endl;
		std::cout << std::endl;
	}
}

//The splitString() function will split the input string line into two words that are separated by a space character.
//Pre: Line string must contain two words separated by the space character.
//Post: Retusn a vector which contain both words.
std::vector<std::string> splitString(std::string line) { // .
	std::vector<std::string> resources(2);
	int counter = 0;
	for (std::string::iterator it = line.begin(); it != line.end(); it++) {
		counter++;
		if (*it == ' ') {
			resources[0] = line.substr(0, counter - 1); //Takes the first resource found on the line and stores it on the resources vector.
			resources[1] = (line).substr(counter, line.size() - 1); // Takes the second resource on the line and stores it on the resources vector.
			break;
		}
	}
	return resources;
}

//The printGraphAdjacencyListRepresentation() function prints the input graph into the screen.
//Pre: None.
//Post: Graph representation output in the screen.
void printGraphAdjacencyListRepresentation(std::unordered_map<std::string, std::list<std::string>> adjacencyList) {
	// Prints the graph by adjacencyList Representation
	std::cout << "~	Graph Representation: " << std::endl;
	std::cout << std::endl;
	std::cout << "Nodes:	    Links: " << std::endl;
	for (auto &value : adjacencyList) {
		std::string vertex = value.first;
		std::list<std::string> adjacentVertices = value.second;
		std::list<std::string>::iterator itr = adjacentVertices.begin();

		std::cout << "[" << vertex << "]";

		while (itr != adjacentVertices.end()) {
			std::cout << " -> " << (*itr);
			++itr;
		}
		std::cout << std::endl;
	}

	std::cout << std::endl;
}

//The graph() function will return the graph as an unordered map, takes the file resource.txt as input to check the links and nodes.
//Pre: file must contain rigth format to build the graph.
//Post: output grap as an unorded map.
std::unordered_map<std::string, std::list<std::string>> graph(std::string fileName, std::vector<Node> &vertices) {
	//Variables declaration
	std::string line;
	std::ifstream resourceFile(fileName); // File which contain the resources and their links. "resource.txt"
	std::vector<std::string> resources(1);
	std::vector<std::string> temporal;
	std::unordered_map<std::string, std::list<std::string>> adjacencyList;
	std::vector<std::string>::iterator it;
	int edges = 0;

	//File reading section
	if (resourceFile) {
		while (getline(resourceFile, line)) {
			edges++; // Each line of the file represents one link of the graph, we count the number of edges as links.
			resources = splitString(line);
			if (vertices.empty()) {
				vertices.push_back(Node(resources[0], true));
				temporal.push_back(resources[0]);
			}
			else {
				for (unsigned int i = 0; i < resources.size(); i++) {

					it = find(temporal.begin(), temporal.end(), resources[i]);
					if (it != temporal.end()) {
						// Vertex was found and is already in the temporal vector that contains all the nodes.
					}
					else {
						vertices.push_back(Node(resources[i], true));
						temporal.push_back(resources[i]);
					}
				}
			}

			adjacencyList[resources[0]].push_back((resources[1])); // Adds the link on the respective node in the graph adjacencyList representation.
		}
	}

	temporal.clear();
	return adjacencyList;
}


int main() {
	//Variables declaration
	std::unordered_map<std::string, std::list<std::string>> adjacencyList;
	std::vector<Node> vertices;
	std::string vertexToDelete;
	std::string command;
	std::string deleteCommand(".delete ");
	std::string insertCommand(".insert ");
	std::string helpCommand(".help");
	std::string displayCommand(".display");
	std::string linkToInsert;
	std::string line;

	//Start Print Commands
	print_commands();

	adjacencyList = graph("resource.txt", vertices);

	//Printing Graph.
	printGraphAdjacencyListRepresentation(adjacencyList);
	printListOfVertices(vertices);

	while (true) {

		print_prompt();
		std::getline(std::cin, command);
		std::cout << std::endl;

		if (command == ".q") { // Quit program.
			std::cin.clear();
			command.clear();
			exit(EXIT_SUCCESS);
		}
		else if (command.compare(0, 6, helpCommand) == 0) { // Show all commands.
			print_commands();
			std::cin.clear();
			command.clear();
		}
		else if (command.compare(0, 9, displayCommand) == 0) { //Display the graph adjacency list representation and the list of vertices and their usability.

			printGraphAdjacencyListRepresentation(adjacencyList);
			printListOfVertices(vertices);

		}
		else if (command.compare(0, 8, deleteCommand) == 0) { //Deleting Vertex.

			vertexToDelete = command.substr(8, command.size());
			deleteVertex(&adjacencyList, &vertices, vertexToDelete);
			printGraphAdjacencyListRepresentation(adjacencyList);
			std::cin.clear();
			command.clear();

		}
		else if (command.compare(0, 8, insertCommand) == 0) { //Inserting link in the "resource.txt" file.

			std::ofstream fileOut("resource.txt", std::ios::app);
			linkToInsert = command.substr(8, command.size());
			bool linkExists = false;
			std::ifstream resourceFile("resource.txt");
			while (!resourceFile.eof()) { // Verifies if the link is already in the file.
				std::getline(resourceFile, line);
				if (line.compare(linkToInsert) == 0) {
					std::cout << "	~The [" << linkToInsert << "] link already exists in the graph." << std::endl;
					linkExists = true;
					break;
				}
			}
			resourceFile.close();
			if (!linkExists) {
				fileOut << linkToInsert << std::endl; //Appends the users input link to the file.
				std::cout << "	~The [" << linkToInsert << "] link has been inserted in the graph." << std::endl;
			}
			fileOut.close();
			std::cin.clear();
			command.clear();
			vertices.clear();
			adjacencyList = graph("resource.txt", vertices); // Loads the graph again with the updated file.
			printGraphAdjacencyListRepresentation(adjacencyList); // Prints the graph.

		}
		else {

			std::cout << "Unrecognized command " << command << "." << std::endl; // If the user types none of the commands enters here.

		}

	}

	return 0;
}