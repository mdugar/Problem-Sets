#ifndef SCENE_H
#define SCENE_H

#include "image.h"
class Scene
{
	private:
		Image** images;
		int** coords;
		int max;
		void copy(const Scene &other);
		void clean();
	public:
		Scene(int max);
		~Scene();
		Scene(const Scene &source);
		const Scene& operator=(Scene const & other);
		void changemaxlayers(int newmax);
		void addpicture(const char* FileName, int index, int x, int y);
		void changelayer(int index, int newindex);
		void translate(int index, int xcoord, int ycoord);
		void deletepicture(int index);
		Image* getpicture(int index) const;
		Image drawscene() const;	

};
#endif
