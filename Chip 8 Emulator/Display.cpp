#include "Display.h"

Window::Window(){
	size[0]=20;
	size[1]=20;
}

void Window::update(){
	char *canvas=new char[this->size[1]+1];
	char *filler=new char[this->size[1]+1];

	for(int i=0;i<this->size[1];i++)
		filler[i]=' ';
	filler[this->size[1]]='\0';
	system("cls");
	for(int y=0;y<this->size[0];y++){

		memcpy(canvas,filler,sizeof(char)*this->size[1]);
		//smallest to largest
		//for(int p=0;this->panes[p]!=nullptr;p++){
		Pane*** cursor;
		while((cursor=panes.iterate())!=nullptr){
			Pane* current=**cursor;
			if(current->scene.topLeft[0]+current->scene.size[0] > y){
				current->render();

				int tmp=(current->scene.size[1] + current->scene.topLeft[1] > current->size[1])?
							current->size[1] - current->scene.topLeft[1] : current->scene.size[1];
				memcpy(canvas,current->scene.landscape[y],sizeof(char)*(tmp>0 ? tmp : 0));
			}
		}
		canvas[this->size[1]]='\0';
		std::cout<<canvas<<std::endl;
	}
	delete[] canvas;
	delete[] filler;
}

void Window::addPane(Pane* toAdd){
	panes.insert/*Sort*/(& toAdd);
}

void Window::addPane(Pane** toAdd){
	panes.insert/*Sort*/(toAdd);
}

Pane::Pane():modified(false),visible(false),height(100){
	this->size[0]=0;
	this->size[1]=0;
	this->scene.topLeft[0]=0;
	this->scene.topLeft[1]=0;
	this->scene.size[0]=0;
	this->scene.size[1]=0;
	this->scene.landscape=nullptr;
}

Pane::Pane(char **input,int size[2]):modified(true),visible(true),height(100){
	this->size[0]=size[0];
	this->size[1]=size[1];
	this->screen=new char*[size[0]];
	for(int i=0;i<size[0];i++){
		this->screen[i]=new char[size[1]];
		for(int j=0;j<size[1];j++){
			this->screen[i][j]=input[i][j];
		}
	}
	this->scene.size[0]=size[0];
	this->scene.size[1]=size[1];
	this->scene.topLeft[0]=0;
	this->scene.topLeft[1]=0;
	this->scene.landscape=nullptr;
}
Pane::~Pane(){
	for(int i=this->size[0];i<this->size[0];i++){
		delete[] this->screen[i];
	}
	delete[] this->screen;

	for(int i=this->scene.size[0];i<this->scene.size[0];i++){
		delete[] this->scene.landscape[i];
	}
	delete[] this->scene.landscape;
}

void Pane::render(){
	if(!modified){
		return;
	}
	if(this->scene.landscape!=nullptr){
		for(int i=0;i<this->scene.size[0];i++){
			delete[] this->scene.landscape[i];
		}
		delete[] this->scene.landscape;
	}
	this->scene.landscape=new char*[size[0]];
    for(int i=this->scene.topLeft[0];i<this->scene.size[0]-this->scene.topLeft[0];i++){

		this->scene.landscape[i]=new char[this->scene.size[1]+1];

		for(int j=this->scene.topLeft[1];j<this->scene.size[1]-this->scene.topLeft[1];j++){

			if(i<this->size[0] && i>=0 && j<this->size[1] && j>=0){
				this->scene.landscape[i][j]=this->screen[i][j];
			}
			else{
				this->scene.landscape[i][j]=(this->filler ? this->filler : '~');
			}
        }
		this->scene.landscape[i][this->scene.size[1]]='\0';
    }
	return;
}

void Pane::setTopLeft(int tmp[2]){
	this->scene.topLeft[0]=tmp[0];
	this->scene.topLeft[1]=tmp[1];
}

int *Pane::getTopLeft() const{
	int *tmp=new int[2];
	tmp[0]=this->scene.topLeft[0];
	tmp[1]=this->scene.topLeft[1];
	return tmp;
}

void Pane::setSceneSize(int tmp[2]){
	this->scene.size[0]=tmp[0];
	this->scene.size[1]=tmp[1];
}

int *Pane::getSceneSize() const{
	int *tmp=new int[2];
	tmp[0]=this->scene.size[0];
	tmp[1]=this->scene.size[1];
	return tmp;
}
void Pane::setSize(int tmp[2]){
	this->size[0]=tmp[0];
	this->size[1]=tmp[1];
}

int *Pane::getSize() const{
	int *tmp=new int[2];
	tmp[0]=this->scene.topLeft[0];
	tmp[1]=this->scene.topLeft[1];
	return tmp;
}

bool operator<(Pane first,Pane second){
	return first.height<second.height;
}
bool operator>(Pane first,Pane second){
	return first.height>second.height;
}
