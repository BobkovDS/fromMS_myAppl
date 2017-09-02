#include "TriangulationDilane.h"
#include <fstream>
#include "stdafx.h"
using namespace std;


std::vector<VertexModelLoader>* Triangle::allVertices = nullptr;

TriangulationDilane::TriangulationDilane(float leftValueOfRangeX, float leftValueOfRangeY, int widthX, int widthY, vector<VertexModelLoader> *_inPutDate)
{
	Triangle::allVertices = _inPutDate;
	inPutDate = _inPutDate;
	LoX = leftValueOfRangeX;
	LoY = leftValueOfRangeY;
	_WidthX = widthX + 1;
	_WidthY = widthY + 1;
}

TriangulationDilane::~TriangulationDilane()
{
}

/*
         ArrayList _date;
        ArrayList _convexHull;
        ArrayList _dbDate;
        System.Drawing.Graphics mGraph;
        ArrayList _Triangl;
        Boolean _state = false;
        bool drawing = false;
        int _Width = 780;
        int IZOCount = 25;
        string mapPath;
        Color izocolor = Color.Red;
        
        //-----DirectX Varible
        private Device device = null;
        private VertexBuffer vb = null;
        private VertexBuffer vb_izo = null;

        bool mDown = false;
        Point iPoint = new Point();
        float rx, ry, rz;
        float mx, my, mz;
        int numVerts = 0;
      
        public void OkButton_Click(object sender, EventArgs e)
        {
            
            GanButton_Click(sender, e);
            drawing = false;
            IZOCount = Convert.ToInt32( tbIZO.Text);
            mGraph = MainCanvas.CreateGraphics();
            mGraph.Clear(Color.Black);
            status_Draw.Visible = false;
            mapPath = mapList.SelectedItem + ".bmp";

            if (_dbDate != null) { _dbDate.Clear(); } else _dbDate = new ArrayList();
  
            if (_state)
            {
                _state = false;
                //_Width = MainCanvas.Width;

                ConvexHullBuilder CNB = new ConvexHullBuilder(_date, MainCanvas);
                _convexHull = CNB._result_convex_hull;
                int t = CNB.GetHashCode();
                _triugol rr = new _triugol();
                InitTr();
                toDelone();

                DELONE_GO();
                status_Draw.Visible = true;
              
                InitializeGraphics();
                DrawIT();
            }
            else MessageBox.Show("Convex Hull was bulit");

        }
        */
void TriangulationDilane::CreateTriangulation(vector<unsigned int> *inPutDate)
{
	
	Triangle newTriangle;

//	newTriangle.SetAdd(inPutDate);
	//newTriangle.Address = inPutDate;

	newTriangle.Vertices[0] = inPutDate->at(0);
	newTriangle.Vertices[1] = inPutDate->at(1);
	newTriangle.Vertices[2] = inPutDate->at(2);

	newTriangle.ID = 0;

	newTriangle.NeighborIDs = { -1, -1, -1 };
	for (int i = 3; i < inPutDate->size(); i++)
	{
		newTriangle.NeighborIDs[1] = newTriangle.ID + 1;
		if (newTriangle.ID != 0) newTriangle.NeighborIDs[2] = newTriangle.ID - 1;
		Triangles.push_back(newTriangle);

		newTriangle.NeighborIDs = { -1, -1, -1 };
		newTriangle.Vertices[0] = inPutDate->at(0);
		newTriangle.Vertices[1] = inPutDate->at(i - 1);
		newTriangle.Vertices[2] = inPutDate->at(i);

		newTriangle.ID = Triangles.size();
	}
	newTriangle.NeighborIDs[2] = newTriangle.ID - 1;
	Triangles.push_back(newTriangle);
	//Triangles.at(0).Index++;
}

void TriangulationDilane::DelonePrepare()
{	
	for (int i = 0; i < Triangles.size(); i++)
	{
		Flip(Triangles.at(i).ID, Triangles.at(i).NeighborIDs[1]);
	}
}

bool TriangulationDilane::Flip(int F, int S)
{
	if (setGlobal) ExportToFile(globalN++);

	if ((F != S) & (S != -1))
	{
		Triangle* triangleA;
		Triangle* triangleB;

		triangleA = &Triangles.at(F);
		triangleB = &Triangles.at(S);
		
		bool _result = false;
		bool Ok = false;
		array<int,2> protiv = GetNomers(F, S);

		int Pr1 = Mod(protiv[0] + 1); // точка "до" в 1
		int Sl1 = Mod(protiv[0]);// точка "после" в 1
		int Pr2 = Mod(protiv[1] + 1); // точка "до" в 2
		int Sl2 = Mod(protiv[1]);// точка "после" в 2

		VertexModelLoader p1 = triangleA->GetVertex(Pr1);
		VertexModelLoader p2 = triangleA->GetVertex(protiv[0]); //<--|
		VertexModelLoader p3 = triangleA->GetVertex(Sl1);       //   |  противоположные вершины
		VertexModelLoader p0 = triangleB->GetVertex(protiv[1]); //<--| 

		double cosA = (p0.x - p1.x) * (p0.x - p3.x) + (p0.y - p1.y) * (p0.y - p3.y);
		double cosB = (p2.x - p1.x) * (p2.x - p3.x) + (p2.y - p1.y) * (p2.y - p3.y);
		if (cosA < 0 & cosB < 0) Ok = true;
		else if (cosA >= 0 & cosB >= 0) Ok = false;
		else
		{
			double Sig =
				((p0.x - p1.x) * (p0.y - p3.y) - (p0.x - p3.x) * (p0.y - p1.y)) * cosB +
				cosA * ((p2.x - p1.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p2.y - p1.y));
			if (Sig < 0) Ok = true; else Ok = false;
		}


		if (Ok)// Выполняется условие, значит надо перестроить
		{
			triangleA->Vertices[Sl1] = triangleB->Vertices[protiv[1]];
			triangleB->Vertices[Sl2] = triangleA->Vertices[protiv[0]];

			triangleA->NeighborIDs[protiv[0]] = triangleB->NeighborIDs[Pr2];
			triangleB->NeighborIDs[protiv[1]] = triangleA->NeighborIDs[Pr1];
			triangleA->NeighborIDs[Pr1] = triangleB->ID;
			triangleB->NeighborIDs[Pr2] = triangleA->ID;

			//меняем указатели в соседях
			Triangle* triangleC;
			array<int, 2> nNew;
			if (triangleB->NeighborIDs[protiv[1]] != -1)
			{
				triangleC = &Triangles.at(triangleB->NeighborIDs[protiv[1]]);
				nNew = GetNomers(triangleB->ID, triangleB->NeighborIDs[protiv[1]]);

				triangleC->NeighborIDs[nNew[1]] = triangleB->ID;
			}

			if (triangleA->NeighborIDs[protiv[0]] != -1)
			{
				triangleC = &Triangles.at(triangleA->NeighborIDs[protiv[0]]);
				nNew = GetNomers(triangleA->ID, triangleA->NeighborIDs[protiv[0]]);
				triangleC->NeighborIDs[nNew[1]] = triangleA->ID;
			}
			_result = true;

			
			Flip(triangleA->ID, triangleA->NeighborIDs[protiv[0]]);

			Flip(triangleB->ID, triangleB->NeighborIDs[Sl2]);
			
		}

		return _result;
	}
	else return false;

}

void TriangulationDilane::ExportToFile(int N)
{
	
	std::string FileName = "outputFile" + std::to_string(N);
	std::ofstream outFile(FileName);

	std::string tmpStr;
	int commCount = 1;
	for (int i = 0; i < Triangles.size(); i++)
	{
		tmpStr = "f " + std::to_string(Triangles.at(i).Vertices[0]+1) + "//" + std::to_string(commCount) + " "
		+ std::to_string(Triangles.at(i).Vertices[1]+1) + "//" + std::to_string(commCount+1) + " "
		+ std::to_string(Triangles.at(i).Vertices[2]+1) + "//" + std::to_string(commCount+2) +"\r\n";
		outFile.write(tmpStr.c_str(), tmpStr.size());
		commCount += 3;
	}
	
	outFile.close();
}
void TriangulationDilane::DeloneIt(vector<uint32_t> *outputIndices)
{
	
	int m = 2, N = 0, R = 6;
	array<uint16_t, 2> CASH;
	vector<uint16_t> tCASH;
	tCASH.resize(m*m);
	
	//CASH[0] = { 0, 0 };
	//CASH[1] = { 0, 0 };
	
	int nTr, oldTr;
	int Row, Column, oldR, oldC;

	
	oldTr = 0;
	oldR = 0; oldC = 0;
	int SoC = m * m * R;
	while (N < inPutDate->size())
	{
	
		VertexModelLoader point = inPutDate->at(N);	
		{
			
			Row = floor((point.x + abs(LoX)) / _WidthX * m);
			Column = floor((point.y + abs(LoY)) / _WidthY * m);
			//nTr = CASH[Row,Column];
			nTr = tCASH.at(Row*m + Column);
		}
	
		array<int,3> Reseach = { 1, nTr,0 };
		while (Reseach[0] == 1) { Reseach = WalkToDarkOfMind(Reseach[1], point); }
		switch (Reseach[0])
		{
		case 0: // Мы нашли точку внутри треугольника
		{
			//CASH[Row, Column] = Reseach[1];
			tCASH.at(Row*m + Column) = Reseach[1];
			uint16_t NewNomer = Triangles.size();
			array<int, 3> Nomera = { Reseach[1], NewNomer, NewNomer + 1 };
			array<int, 2> nnom;

			Triangle AddingT = Triangle();
			Triangle DividT = Triangles.at(Reseach[1]);			

			//First Added		
			
			AddingT.ID = NewNomer;
			AddingT.Vertices[0] = DividT.Vertices[1];
			AddingT.Vertices[1] = N;//AddingT.SetPoint(1, point);
			AddingT.Vertices[2] = DividT.Vertices[0];
			uint16_t tNewNomer = NewNomer + 1;
			AddingT.NeighborIDs = { DividT.ID, DividT.NeighborIDs[2],tNewNomer };
			Triangles.push_back(AddingT);
			
			if (AddingT.NeighborIDs[1] != -1)
			{
				//Neigh = &Triangles.at(AddingT.NeighborIDs[1]);
				nnom = GetNomers(NewNomer, AddingT.NeighborIDs[1]);
				Triangles.at(AddingT.NeighborIDs[1]).NeighborIDs[nnom[1]] = NewNomer;
			}
			
			AddingT.ID = NewNomer + 1;
			AddingT.Vertices[0] = DividT.Vertices[2];
			AddingT.Vertices[1] = N;
			AddingT.Vertices[2] = DividT.Vertices[1];
			AddingT.NeighborIDs = { NewNomer, DividT.NeighborIDs[0], DividT.ID };
			Triangles.push_back(AddingT);

			if (AddingT.NeighborIDs[1] != -1)
			{
				//Neigh = &Triangles.at(AddingT.NeighborIDs[1]);
				nnom = GetNomers(NewNomer + 1, AddingT.NeighborIDs[1]);
				Triangles.at(AddingT.NeighborIDs[1]).NeighborIDs[nnom[1]] = NewNomer + 1;
			}

			// modifed found treugol
			DividT.Vertices[1] = N;
			DividT.NeighborIDs[0] = NewNomer + 1;
			DividT.NeighborIDs[2] = NewNomer;

			Triangles.at(DividT.ID) = DividT;
			/*
			for (int i = 0; i < 3; i++)
			{
				_triugol work = (_triugol)_Triangl[Nomera[i]];
				//ok = Flip(work.ID, work.NeighborIDs[0]);
				Flip(work.ID, work.NeighborIDs[1]);
				// ok |= Flip(work.ID, work.NeighborIDs[2]);
			}*/

			int Stop = 0;
			if (N == 14)
			{
				Stop = 1;				
			}
			Triangle work = Triangles.at(Reseach[1]);
			Flip(work.ID, work.NeighborIDs[1]);

			if (Stop == 1) setGlobal = true; else setGlobal = false;

			work = Triangles.at(NewNomer);
			Flip(work.ID, work.NeighborIDs[1]);

			work = Triangles.at(NewNomer + 1);
			Flip(work.ID, work.NeighborIDs[1]);
			break;
		}
		case 2:// Мы нашили раннее введенную вершину
		{			
			//CASH[Row][Column] = Reseach[1];
			tCASH.at(Row*m + Column) = Reseach[1];
			break;
		}
		case 3: //мы нашли точку на ребре
		{
			//CASH[Row][ Column] = Reseach[2];
			tCASH.at(Row*m + Column) = Reseach[2];
			int ver = Reseach[1];
			int Sl = Mod(ver);
			int Pr = Mod(ver + 1);

			Triangle Our = Triangles.at(Reseach[2]);
			Triangle tmpOur = Our; // так как пока с указателями на Vector не разобрался, то вводим доп переенную которая в зависимости от ветвления будет указывать для последних flip текущий расщепляемый треугольник
			Triangle AddingT;
			uint16_t NewNomer = Triangles.size();
			
			//Adding First new triangl
			AddingT.ID = NewNomer;
			AddingT.Vertices[0] = Our.Vertices[Pr];
			AddingT.Vertices[1] = Our.Vertices[ver];
			AddingT.Vertices[2] = N;

			//Modify Our treangle

			//---------------------------------------------------------------
			// Ok
			if (Our.NeighborIDs[Pr] != -1)
			{
				Triangle Neigh =Triangles.at(Our.NeighborIDs[Pr]);
				array<int, 2> nnom = GetNomers(Our.ID, Neigh.ID);
				int tNewNomer = NewNomer+1;

				AddingT.NeighborIDs = { tNewNomer, Our.ID, Our.NeighborIDs[Sl] };
				Triangles.push_back(AddingT);;
				if (AddingT.NeighborIDs[2] != -1)
				{
					Triangle T3 =Triangles.at(AddingT.NeighborIDs[2]);
					array<int, 2> nNew = GetNomers(AddingT.ID, T3.ID);					
					Triangles.at(AddingT.NeighborIDs[2]).NeighborIDs[nNew[1]] = AddingT.ID;
				}

				Triangles.at(Reseach[2]).Vertices[ver] = N; // -------- WHY IT HERE ? !!!!!!!!!!!!!!!!!!!!!!!!! 
				Triangles.at(Reseach[2]).NeighborIDs[Sl] = AddingT.ID;
								
				tmpOur = Neigh;
				ver = Mod(nnom[1] + 1);
				Sl = nnom[1];
				Pr = Mod(ver + 1);

				////Adding Second new triangl
				
				AddingT.ID = NewNomer + 1;
				AddingT.Vertices[0] = tmpOur.Vertices[Sl];
				AddingT.Vertices[1] = N;
				AddingT.Vertices[2] = tmpOur.Vertices[ver];
				AddingT.NeighborIDs = { NewNomer, tmpOur.NeighborIDs[Pr], tmpOur.ID, };
				Triangles.push_back(AddingT);;
				//Modify Our treangle
				tmpOur.Vertices[ver] = N;
				tmpOur.NeighborIDs[Pr] = AddingT.ID;
				Triangles.at(tmpOur.ID) = tmpOur;

				if (AddingT.NeighborIDs[1] != -1)
				{
					Triangle T3 =Triangles.at(AddingT.NeighborIDs[1]);
					array<int, 2> nNew = GetNomers(AddingT.ID, T3.ID);
					Triangles.at(AddingT.NeighborIDs[1]).NeighborIDs[nNew[1]] = AddingT.ID;
				}
			}
			else
			{

				AddingT.NeighborIDs = { -1, Our.ID, Our.NeighborIDs[Sl] };   
				Triangles.push_back(AddingT);;
				if (AddingT.NeighborIDs[2] != -1)
				{
					Triangle T3 =Triangles.at(AddingT.NeighborIDs[2]);
					array<int, 2>  nNew = GetNomers(AddingT.ID, T3.ID);
					Triangles.at(AddingT.NeighborIDs[2]).NeighborIDs[nNew[1]] = AddingT.ID;
				}
				Triangles.at(Reseach[2]).Vertices[ver] = N;
				Triangles.at(Reseach[2]).NeighborIDs[Sl] = AddingT.ID;
			}

			//Flip(AddingT.ID, AddingT.NeighborIDs[0]);
			Flip(AddingT.ID, AddingT.NeighborIDs[1]);
			Flip(AddingT.ID, AddingT.NeighborIDs[2]);
			Flip(tmpOur.ID, tmpOur.NeighborIDs[0]);
			Flip(tmpOur.ID, tmpOur.NeighborIDs[1]);
			Flip(tmpOur.ID, tmpOur.NeighborIDs[2]);

			break;
		}
		}
		/* oldPoint = point;
		 oldTr = nTr;
		 oldC = Column;
		 oldR = Row;*/
		 // Point is added
		ExportToFile(N);
		N++;
		if (N == SoC)
		{			
			m *= 2;
			tCASH = CreateDinamicCAHS(tCASH, m);			
			SoC = m * m * R;
		}
	}
	
	// Here triangulation Delone is ready. Build Indices set

	for (int i = 0; i < Triangles.size(); i++)
	{
		outputIndices->push_back(Triangles.at(i).Vertices[0]);
		outputIndices->push_back(Triangles.at(i).Vertices[1]);
		outputIndices->push_back(Triangles.at(i).Vertices[2]);
	}

}

array<int, 3> TriangulationDilane::WalkToDarkOfMind(int nomer, VertexModelLoader P)
{
	/*
		Функция обанружения заданной точки P относительно ребер треугольника Nomer. 
		Точка может:
		- Лежать внутри треугольника. Возвращается тип результата - 0 и номер треугольника
		- Лежать на одной из граней. Возвращается тип результата - 3 и номер вершины начала ребра и номер треугольника
		- Лежать вне треугольника (слева или справа от ребра, зависит от обхода). Возвращается тип результата - 1 и номер 
		соседнего треугольника в котором может находится искомая точка

	*/
	Triangle cTr = Triangles.at(nomer);

	VertexModelLoader p0 = cTr.GetVertex(0);
	VertexModelLoader p1 = cTr.GetVertex(1);
	VertexModelLoader p2 = cTr.GetVertex(2);
	double Epss = 0.000000001;


	if (P != p0 & P != p1 & P != p2)
	{
		int ver = 2;
		double sin2 = SIN(P, p0, p1);
		//double sin1 = SIN(p0, p1, p2);
		//<= Epss
		if (abs(sin2) <= Epss) { return { 3, 0, cTr.ID }; }
		else
			if (sin2 < 0) { return { 1, cTr.NeighborIDs[ver],0 }; }
			else
			{
				ver = 0;
				sin2 = SIN(P, p1, p2);
				if (abs(sin2) <= Epss) { return { 3, 1, cTr.ID }; }
				else
					if (sin2 < 0) { return { 1, cTr.NeighborIDs[ver] }; }
					else
					{
						ver = 1;
						sin2 = SIN(P, p2, p0);
						if (abs(sin2) <= Epss) { return { 3, 2, cTr.ID }; }
						else
							if (sin2 < 0) { return  { 1, cTr.NeighborIDs[ver] }; }
							else
								return { 0, cTr.ID };
					}
			}
	}
	else return { 2, cTr.ID, 0 };
}

//double TriangulationDilane::SIN(VertexModelLoader p0, VertexModelLoader p1, VertexModelLoader p2)
double TriangulationDilane::SIN(VertexModelLoader p, VertexModelLoader a, VertexModelLoader b)
{
	VertexModelLoader p1 = p - a;
	VertexModelLoader p2 = b - a;
	//return (p0.x - p1.x) * (p0.y - p2.y) - (p0.x - p2.x) * (p0.y - p1.y);
	return p1.x*p2.y - p1.y*p2.x;
}

vector<uint16_t> TriangulationDilane::CreateDinamicCAHS(vector<uint16_t> oldCASH, int oldM)
{
	vector<uint16_t> nCASH;
	nCASH.resize(oldM*oldM);

	for (int i = 0; i < oldM; i++)
		for (int j = 0; j < oldM; j++)

			nCASH[2 * i, 2 * j] = nCASH[2 * i, 2 * j + 1] =
			nCASH[2 * i + 1, 2 * j] = nCASH[2 * i + 1, 2 * j + 1] = oldCASH[i, j];

	return nCASH;
}
/*
        void Gan(int r)
        {
            _state = true;
            _date = new ArrayList();

            int Number = Convert.ToInt32(inNumber.Text);
            Random _random = new Random();
            if (r == 1)
            {
                _date.Add(new _point(20, 200));
                _date.Add(new _point(500, 50));
                _date.Add(new _point(460, 550));
            }
            else
            {
                for (int i = 0; i < Number; i++)
                {
                    _point addPoint = new _point(_random.NextDouble() * _Width, _random.NextDouble() * _Width);
                    _date.Add(addPoint);
                }
            }
        }
        //void GanButton_Click(object sender, EventArgs e)
		void GanButton_Click()
        {
            Gan(0);
        }
*/
        // helpers
uint16_t TriangulationDilane::Mod(uint16_t v)
{
	switch (v)
	{
	case 0: return 1;
	case 1: return 2;
	case 2: return 0;
	case 3: return 1;
	case 4: return 2;
	case 5: return 0;
	}
	return 0;

}
        
array<int, 2>  TriangulationDilane::GetNomers(int f, int s)
{
	// Получаем номера противоположныж вершин смежных треугольников
	// f - номер 1 треугольника
	// s - номер 2 треугольника
	// v1 - номер вершины в 1 треугольнике           
	// v2 - номер вершины в 2 треугольнике

	Triangle First = Triangles.at(f);
	Triangle Second = Triangles.at(s);
	array<int, 2> result;
	VertexModelLoader p1_0, p1_1, p1_2;
	VertexModelLoader p2_0, p2_1, p2_2;

	p1_0 = First.GetVertex(0);
	p1_1 = First.GetVertex(1);
	p1_2 = First.GetVertex(2);

	p2_0 = Second.GetVertex(0);
	p2_1 = Second.GetVertex(1);
	p2_2 = Second.GetVertex(2);

	VertexModelLoader p0 = p2_2;
	int i = 2;
	while (p0 == p1_0 | p0 == p1_1 | p0 == p1_2) { i--; p0 = Second.GetVertex(i); }
	result[1] = i;

	p0 = p1_2;
	i = 2;
	while (p0 == p2_0 | p0 == p2_1 | p0 == p2_2) { i--; p0 = First.GetVertex(i); }
	result[0] = i;

	//return new int[] {v1,v2};
	return result;
}

bool TriangulationDilane::_Nvertex(int _index, int v, int N)
{
	// _inedx - номер текущего треугольника
	// v - номер текущей проверяемой вершины
	// N - номер текущего соседа

	bool error = false;
	if (N != -1)
	{
		int s1 = 0, s2 = 0;
		switch (v)
		{
		case 0: { s1 = 1; s2 = 2; break; }
		case 1: { s1 = 0; s2 = 2; break; }
		case 2: { s1 = 0; s2 = 1; break; }
		}

		Triangle Neig = Triangles.at(N);
		Triangle cTr = Triangles.at(_index);
		int i = 0;
		while ((Neig.Vertices[i] == cTr.Vertices[s1]) |
			(Neig.Vertices[i] == cTr.Vertices[s2])) {
			i++;
		}
		if (Neig.NeighborIDs[i] != _index) error = true;
	}
	return error;
}

        void TriangulationDilane::DebugFunction()
        {            bool myERROR, error1, error2, error3;
			myERROR = error3 = error1 = error2 = false;
            for (int i = 0; i < Triangles.size(); i++)
            {
				Triangle ctr =Triangles.at(i);
                error1 = _Nvertex(i, 0, ctr.NeighborIDs[0]);
                error2 = _Nvertex(i, 1, ctr.NeighborIDs[1]);
                error3 = _Nvertex(i, 2, ctr.NeighborIDs[2]);
				myERROR = myERROR | error1 | error2 | error3;
            }
            if (myERROR)
            {
                MessageBox(0, L"Error. Неправильные соседи",0,0);
				
            }

            for (int i = 0; i < Triangles.size(); i++)
            {
				Triangle work =Triangles.at(i);
				std::wstring text;
				
                if (Flip(work.ID, work.NeighborIDs[0])) MessageBox(0, L"0", 0, 0);

                if (Flip(work.ID, work.NeighborIDs[1])) MessageBox(0, L"1", 0, 0);

                if (Flip(work.ID, work.NeighborIDs[2])) MessageBox(0, L"2", 0, 0);				
            }
        }
		/*
        //----------IZOLINE
        private ArrayList Metka_of_Hlayer(float h)
        {
            int onepoint = 0;
            ArrayList izoLine = new ArrayList();
            _triugol t = new _triugol();
            double z1, z2, z3;
            for (int i = 0; i < _Triangl.Count; i++)
            {
                t =Triangles.at(i];
                z1 = t.Vertices[0].Z;
                z2 = t.Vertices[1].Z;
                z3 = t.Vertices[2].Z;
                double min = Math.Min(Math.Min(z1, z2), z3);
                double max = Math.Max(Math.Max(z1, z2), z3);
                if (min < h & h < max)
                {
                   
                    t.flagC = h;
                    if ((z1 <= h & h < z2) | (z2 < h & h <= z1)) { onepoint++; izoLine.Add(GetIzoPoint(t.Vertices[1], t.Vertices[0], h)); }
                    if ((z2 <= h & h < z3) | (z3 < h & h <= z2)) { onepoint++; izoLine.Add(GetIzoPoint(t.Vertices[2], t.Vertices[1], h)); }
                    if ((z3 <= h & h < z1) | (z1 < h & h <= z3)) { onepoint++; izoLine.Add(GetIzoPoint(t.Vertices[0], t.Vertices[2], h)); }
                   
                    onepoint = 0;
                }
            }
            return izoLine;
        }
        private _point GetIzoPoint(_point ip1, _point ip2, float h)
        {
            _point p1 = new _point(), p2= new _point();
            // p1 выше чем p2
            if (ip1.Z > ip2.Z)
            {
                p1 = new _point(ip1.x / _Width, ip1.y / _Width,ip1.Z);
                p2 = new _point(ip2.x / _Width, ip2.y / _Width,ip2.Z);
            }
            else
            {
                p1 = new _point(ip2.x / _Width, ip2.y / _Width,ip2.Z);
                p2 = new _point(ip1.x / _Width, ip1.y / _Width,ip1.Z);
            }
          
            //p1.x = p1.x / _Width; p2.x = p2.x / _Width;
            //p1.y = p1.y / _Width; p2.y = p2.y / _Width;

            double Z = p1.Z - p2.Z;
            double Zs = h - p2.Z;
            double X = Math.Abs(p1.x - p2.x);
            double Y = Math.Abs(p1.y - p2.y);
            double _x = X * Zs / Z;
            double _y = Y * Zs / Z;


            if (p1.x > p2.x) _x += p2.x; else _x = p2.x - _x;
            if (p1.y > p2.y) _y += p2.y; else _y = p2.y - _y;
            return new _point(_x, _y, h);
            
        }
		*/
        
		/*
        //--------------------------------DirectX---------------------------
        public void InitializeGraphics()
        {
            // Set our presentation parameters
            PresentParameters presentParams = new PresentParameters();

            presentParams.Windowed = true;
            presentParams.SwapEffect = SwapEffect.Discard;

            // Create our device
            device = new Device(0, DeviceType.Hardware, MainCanvas.Handle,
               CreateFlags.SoftwareVertexProcessing, presentParams);
            //Create our VertexBuffer
            vb = new VertexBuffer(typeof(CustomVertex.PositionColored), _Triangl.Count*3, device,
                Usage.Dynamic | Usage.WriteOnly,
                CustomVertex.PositionColored.Format, Pool.Default);
            drawing = true;
            vb.Created += new EventHandler(this.onVertexBufferCreate);
            onVertexBufferCreate(vb, null);
        }
        private void onVertexBufferCreate(object sender, EventArgs e)
        {
            Bitmap map = new Bitmap(mapPath);
            double x, y, z, k = 760;
            Color _color;
            _triugol t = new _triugol();
            VertexBuffer buffer = (VertexBuffer)sender;
            CustomVertex.PositionColored[] vertex = new CustomVertex.PositionColored[_Triangl.Count * 3];
            for (int i = 0; i < _Triangl.Count; i++)
            {
                t =Triangles.at(i];
                x = t.Vertices[0].x / _Width;
                y = t.Vertices[0].y / _Width;
                _color = map.GetPixel((int)(x * 99), (int)(y * 99));
                z = (_color.R + _color.G + _color.B) / k;
                t.Vertices[0].Z = z;
                vertex[i * 3].Position = new Vector3((float)x, (float)y, (float)z);

                x = t.Vertices[1].x / _Width;
                y = t.Vertices[1].y / _Width;
                _color = map.GetPixel((int)(x * 99), (int)(y * 99));
                z = (_color.R + _color.G + _color.B) / k;
                t.node[1].Z = z;
                vertex[i * 3 + 1].Position = new Vector3((float)x, (float)y, (float)z);

                x = t.node[2].x / _Width;
                y = t.node[2].y / _Width;
                int cR = (int)(x * 100);
                int cL = (int)(y * 100);
                _color = map.GetPixel(cR, cL);
                z = (_color.R + _color.G + _color.B) / k;
                t.node[2].Z = z;
                vertex[i * 3 + 2].Position = new Vector3((float)x, (float)y, (float)z);

                Color pColor = Color.LightGray;
                vertex[i * 3].Color = pColor.ToArgb();
                vertex[i * 3 + 1].Color = pColor.ToArgb();
                vertex[i * 3 + 2].Color = pColor.ToArgb();
            }
            buffer.SetData(vertex, 0, LockFlags.None);

            //Create IZOLIne here
            
            ArrayList IZOLines = new ArrayList();
            ArrayList IL = new ArrayList();
            float h = 1 / (float)IZOCount;
            float H = h;

            for (int i = 0; i < IZOCount; i++)
            {
                IZOLines.Add(Metka_of_Hlayer(H));
                H += h;
            }

            numVerts = 0;
            for (int i = 0; i < IZOLines.Count; i++)
            {
                ArrayList addIzo = (ArrayList)IZOLines[i];
                numVerts += addIzo.Count  ;
            }
           
            CustomVertex.PositionColored[] izoverts = new CustomVertex.PositionColored[numVerts];
            int cNum = 0;
            for (int i = 0; i < IZOLines.Count; i++)
            {
                IL = (ArrayList)IZOLines[i];

                int c1 = Color.Green.ToArgb();
                int c2 = Color.Red.ToArgb();
                int c3 = Color.Blue.ToArgb();
                int c4 = Color.Yellow.ToArgb();
                
                for (int p = 0; p < IL.Count; p++)
                {
                    _point izpoint = (_point)IL[p];
                    izoverts[cNum].Position = new Vector3((float)izpoint.x, (float)izpoint.y, (float)izpoint.Z);
                    float Dc = IL.Count / (i+1);
                    //izoverts[cNum].Color = izocolor.ToArgb() +(int)Dc * 1000;
                    izoverts[cNum].Color = izocolor.ToArgb() - 65000 * i / IL.Count;
                    cNum++;
                }
            }
            if (numVerts != 0)
            {
                vb_izo = new VertexBuffer(typeof(CustomVertex.PositionColored), numVerts, device,
                    Usage.Dynamic | Usage.WriteOnly, CustomVertex.PositionColored.Format, Pool.Default);
                vb_izo.SetData(izoverts, 0, LockFlags.None);
            }
        }
        protected void DrawIT()
        {
            
            _point izpoint = new _point();
            device.Clear(ClearFlags.Target, Color.Black, 1.0f, 0);
            device.Present();
            SetupCamera();
            device.RenderState.CullMode = Cull.None;
            device.RenderState.FillMode = FillMode.WireFrame;
            
            CustomVertex.PositionColored[] osi = new CustomVertex.PositionColored[6];
            //x
            osi[0].Position = new Vector3(-1, 0, 0);
            osi[1].Position = new Vector3(1, 0, 0);
            osi[0].Color = Color.Yellow.ToArgb();
            osi[1].Color = Color.Red.ToArgb();
            //y
            osi[2].Position = new Vector3(0, -1, 0);
            osi[3].Position = new Vector3(0, 1, 0);
            osi[2].Color = Color.Yellow.ToArgb();
            osi[3].Color = Color.Blue.ToArgb();
            //z
            osi[4].Position = new Vector3(0, 0, -1);
            osi[5].Position = new Vector3(0, 0, 1);
            osi[4].Color = Color.Yellow.ToArgb();
            osi[5].Color = Color.Green.ToArgb();

            device.BeginScene();
            device.VertexFormat = CustomVertex.PositionColored.Format;
 
            device.DrawUserPrimitives(PrimitiveType.LineList, 3, osi);
            
            device.SetStreamSource(0, vb, 0);
            device.DrawPrimitives(PrimitiveType.TriangleList, 0, _Triangl.Count);

            if (vb_izo != null)
            {
                device.SetStreamSource(0, vb_izo, 0);
                device.DrawPrimitives(PrimitiveType.LineList, 0, numVerts / 2);
            }
            device.EndScene();
            device.Present();
            
        }



 
        
        //------------------------------------------------------------------
    
	*/