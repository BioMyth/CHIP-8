#pragma once
#ifndef _WINDOW_H_
#define _WINDOW_H_
//#include "Map.h"
//#include "DLL.h"
#include "SL.h"
#include <iostream>
#include <cstring>

class Pane{
public:
	friend class Window;
	Pane();
	Pane(char **input,int size[2]);
	~Pane();

	void  render();

	void setTopLeft(int tmp[2]);
	int* getTopLeft() const;

	void setSceneSize(int tmp[2]);
	int* getSceneSize() const;

	void setSize(int tmp[2]);
	int* getSize() const;

	friend bool operator<(Pane first,Pane second);
	friend bool operator>(Pane first,Pane second);

	char filler;
private:
	struct Scene{
		char **landscape;
		int size[2];
		int topLeft[2];
	};
	Scene scene;
	bool visible;
	bool modified;		//alows for individual pane updates instead of always updating the full screen

	int size[2];
	char **screen;

	//int topLeft[2],screenSize[2];
	int height;
};


class Window{
public:
	Window();
    void update();
//    void newMap(Map* newMap);
	void addPane(Pane* toAdd);
	void addPane(Pane** toAdd);

private:
	void cursorX(int in);
	void cursorY(int in);
	void cursorXY(int in[2]);
	// entites have weight(aka are above the map pane but below all other panes) and any menu indicators are additional panes
	// there will be a system to generate a pane out of the list of entities(or they will be on their own pane)
	//DoublyLinkedList<Pane**> panes;
	SortedList<Pane**> panes;
    int size[2];
};

#endif
