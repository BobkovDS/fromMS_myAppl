


    public partial class ConvexHullApl : Form
    {
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
      
        public ConvexHullApl()
        {
            //this.SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.Opaque, true);
            InitializeComponent();
        }

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
        
        void InitTr()
        {
            _Triangl = new ArrayList();

            _triugol tTr = new _triugol();

            tTr.SetPoint(0, _convexHull[0]);
            tTr.SetPoint(1, _convexHull[1]);
            tTr.SetPoint(2, _convexHull[2]);
            tTr.index = _Triangl.Count;

            for (int i = 3; i < _convexHull.Count; i++)
            {
                tTr.triangtes[1] = tTr.index + 1;
                if (tTr.index != 0) tTr.triangtes[2] = tTr.index - 1;
                _Triangl.Add(tTr);

                tTr = new _triugol();
                tTr.SetPoint(0, _convexHull[0]);
                tTr.SetPoint(1, _convexHull[i - 1]);
                tTr.SetPoint(2, _convexHull[i]);
                tTr.index = _Triangl.Count;


            }
            tTr.triangtes[2] = tTr.index - 1;
            _Triangl.Add(tTr);

        }
        void toDelone()
        {

            for (int i = 0; i < _Triangl.Count; i++)
            {

                _triugol work = (_triugol)_Triangl[i];

                //000000000 while (Flip(work.index, work.triangtes[0])) { };
                Flip(work.index, work.triangtes[1]);
                // while (Flip(work.index, work.triangtes[1])) { };

                //000000000while (Flip(work.index, work.triangtes[2])) { };

            }

        }
        bool Flip(int F, int S)
        {
            if ((F != S) & (S != -1))
            {
                _triugol T1 = (_triugol)_Triangl[F];
                _triugol T2 = (_triugol)_Triangl[S];
                bool _result = false;
                bool Ok = false;
                int[] protiv = GetNomers(F, S);
                int Pr1 = _mod(protiv[0] + 1); // точка "до" в 1
                int Sl1 = _mod(protiv[0]);// точка "после" в 1
                int Pr2 = _mod(protiv[1] + 1); // точка "до" в 2
                int Sl2 = _mod(protiv[1]);// точка "после" в 2

                _point p1 = T1.node[Pr1];
                _point p2 = T1.node[protiv[0]]; //<--|
                _point p3 = T1.node[Sl1];       //   |  противоположные вершины
                _point p0 = T2.node[protiv[1]]; //<--| 

                double cosA = (p0.x - p1.x) * (p0.x - p3.x) + (p0.y - p1.y) * (p0.y - p3.y);
                double cosB = (p2.x - p1.x) * (p2.x - p3.x) + (p2.y - p1.y) * (p2.y - p3.y);
                if (cosA > 0 & cosB > 0) Ok = false;
                else if (cosA <= 0 & cosB <= 0) Ok = true;
                else
                {
                    double Sig =
                             ((p0.x - p1.x) * (p0.y - p3.y) - (p0.x - p3.x) * (p0.y - p1.y)) * cosB -
                             cosA * ((p2.x - p1.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p2.y - p1.y));
                    if (Sig <= 0) Ok = true; else Ok = false;
                }


                if (Ok)// ¬ыполн€етс€ условие, значит надо перестроить
                {
                    T1.node[Sl1] = p0;
                    T2.node[Sl2] = p2;

                    T1.triangtes[protiv[0]] = T2.triangtes[Pr2];
                    T2.triangtes[protiv[1]] = T1.triangtes[Pr1];
                    T1.triangtes[Pr1] = T2.index;
                    T2.triangtes[Pr2] = T1.index;

                    //мен€ем указатели в сосед€х
                    _triugol T3;
                    int[] nNew;
                    if (T2.triangtes[protiv[1]] != -1)
                    {
                        T3 = (_triugol)_Triangl[T2.triangtes[protiv[1]]];
                        nNew = GetNomers(T2.index, T2.triangtes[protiv[1]]);
                       
                        T3.triangtes[nNew[1]] = T2.index;
                    }

                    if (T1.triangtes[protiv[0]] != -1)
                    {
                        T3 = (_triugol)_Triangl[T1.triangtes[protiv[0]]];
                        nNew = GetNomers(T1.index, T1.triangtes[protiv[0]]);
                        T3.triangtes[nNew[1]] = T1.index;
                    }
                    _result = true;

                   // Flip(T1.index, T1.triangtes[Sl1]);
                    Flip(T1.index, T1.triangtes[protiv[0]]);

                    Flip(T2.index, T2.triangtes[Sl2]);
                    //Flip(T2.index, T2.triangtes[protiv[1]]);
                }

                return _result;
            }
            else return false;

        }
        private void DELONE_GO()
        {
            int m = 2, N = 0, R = 6;

            int[,] CASH = new int[,] { { 0, 0 }, { 0, 0 } };
            int nTr, oldTr;
            int Row, Column, oldR, oldC;
            _point oldPoint = (_point)_date[0];
            oldTr = 0;
            oldR = 0; oldC = 0;
            int SoC = m * m * R;
            while (N < _date.Count)
            {
                _point point = (_point)_date[N];
               /* if (Math.Sqrt((oldPoint.x - point.x) * (oldPoint.x - point.x) + (oldPoint.y - point.y) * (oldPoint.y - point.y)) < _Width / m)
                {
                    nTr = oldTr;
                    Row = oldR;
                    Column = oldC;
                }
                else*/
                {
                    Row = (int)Math.Floor(point.x / _Width * m);
                    Column = (int)Math.Floor(point.y / _Width * m);
                    nTr = CASH[Row, Column];
                }
                int[] Reseach = new int[] { 1, nTr };
                while (Reseach[0] == 1) { Reseach = WalkToDarkOfMind(Reseach[1], point); }
                switch (Reseach[0])
                {
                    case 0: // ћы нашли точку внутри треугольника
                        {
                            CASH[Row, Column] = Reseach[1];
                            int NewNomer = _Triangl.Count;
                            int[] Nomera = new int[] { Reseach[1], NewNomer, NewNomer + 1 };
                            int[] nnom;

                            _triugol AddingT;
                            _triugol DividT = (_triugol)_Triangl[Reseach[1]];
                            _triugol Neigh = new _triugol();

                            //First Added
                            AddingT = new _triugol();
                            AddingT.index = NewNomer;
                            AddingT.node[0] = DividT.node[1];
                            AddingT.node[1] = point;//AddingT.SetPoint(1, point);
                            AddingT.node[2] = DividT.node[0];
                            AddingT.triangtes = new int[] { DividT.index, DividT.triangtes[2], NewNomer + 1 };
                            _Triangl.Add(AddingT);

                            if (AddingT.triangtes[1] != -1)
                            {
                                Neigh = (_triugol)_Triangl[AddingT.triangtes[1]];
                                nnom = GetNomers(NewNomer, AddingT.triangtes[1]);
                                Neigh.triangtes[nnom[1]] = NewNomer;
                            }
                            AddingT = new _triugol();
                            AddingT.index = NewNomer + 1;
                            AddingT.node[0] = DividT.node[2];
                            AddingT.node[1] = point;
                            AddingT.node[2] = DividT.node[1];
                            AddingT.triangtes = new int[] { NewNomer, DividT.triangtes[0], DividT.index };
                            _Triangl.Add(AddingT);

                            if (AddingT.triangtes[1] != -1)
                            {
                                Neigh = (_triugol)_Triangl[AddingT.triangtes[1]];
                                nnom = GetNomers(NewNomer + 1, AddingT.triangtes[1]);
                                Neigh.triangtes[nnom[1]] = NewNomer + 1;
                            }

                            // modifed found treugol
                            DividT.node[1] = point;
                            DividT.triangtes[0] = NewNomer + 1;
                            DividT.triangtes[2] = NewNomer;

                            /*
                            for (int i = 0; i < 3; i++)
                            {
                                _triugol work = (_triugol)_Triangl[Nomera[i]];
                                //ok = Flip(work.index, work.triangtes[0]);
                                Flip(work.index, work.triangtes[1]);
                                // ok |= Flip(work.index, work.triangtes[2]);
                            }*/
                            _triugol work = (_triugol)_Triangl[Reseach[1]];
                            Flip(work.index, work.triangtes[1]);
                            
                            work = (_triugol)_Triangl[NewNomer];
                            Flip(work.index, work.triangtes[1]);
                            
                            work = (_triugol)_Triangl[NewNomer + 1];
                            Flip(work.index, work.triangtes[1]);
                            break;
                        }
                    case 2:// ћы нашили раннее введенную вершину
                        {
                            CASH[Row, Column] = Reseach[1];
                            break;
                        }
                    case 3: //мы нашли точку на ребре
                        {
                            CASH[Row, Column] = Reseach[2];
                            int ver = Reseach[1];
                            int Sl = _mod(ver);
                            int Pr = _mod(ver + 1);

                            _triugol Our = (_triugol)_Triangl[Reseach[2]];
                            _triugol AddingT;
                            int NewNomer = _Triangl.Count;
                            
                            
                            //Adding First new triangl
                            AddingT = new _triugol();
                            AddingT.index = NewNomer ;
                            AddingT.node[0] = Our.node[Pr];
                            AddingT.node[1] = Our.node[ver];
                            AddingT.node[2] = point;
                           
                            //Modify Our treangle
                                                              
                            //---------------------------------------------------------------
                            // Ok
                            if (Our.triangtes[Pr] != -1)
                            {
                                _triugol Neigh = (_triugol)_Triangl[Our.triangtes[Pr]];
                                int[] nnom = GetNomers(Our.index, Neigh.index);

                                AddingT.triangtes = new int[] { NewNomer + 1, Our.index, Our.triangtes[Sl] };
                                _Triangl.Add(AddingT);
                                if (AddingT.triangtes[2] != -1)
                                {
                                    _triugol T3 = (_triugol)_Triangl[AddingT.triangtes[2]];
                                    int[] nNew = GetNomers(AddingT.index, T3.index);
                                    T3.triangtes[nNew[1]] = AddingT.index;
                                }
                                
                                Our.node[ver] = point;
                                Our.triangtes[Sl] = AddingT.index;
          
                                Our = new _triugol();
                                Our = Neigh;
                                ver = _mod(nnom[1] + 1);
                                Sl = nnom[1];
                                Pr = _mod(ver + 1);

                                ////Adding Second new triangl
                                AddingT = new _triugol();
                                AddingT.index = NewNomer + 1;
                                AddingT.node[0] = Our.node[Sl];
                                AddingT.node[1] = point;
                                AddingT.node[2] = Our.node[ver];
                                AddingT.triangtes = new int[] { NewNomer, Our.triangtes[Pr], Our.index, };
                                _Triangl.Add(AddingT);
                                //Modify Our treangle
                                Our.node[ver] = point;
                                Our.triangtes[Pr] = AddingT.index;

                                if (AddingT.triangtes[1] != -1)
                                {
                                    _triugol T3 = (_triugol)_Triangl[AddingT.triangtes[1]];
                                    int[] nNew = GetNomers(AddingT.index, T3.index);
                                    T3.triangtes[nNew[1]] = AddingT.index;
                                }
                            }
                            else
                            {

                                AddingT.triangtes = new int[] { -1, Our.index, Our.triangtes[Sl] };
                                _Triangl.Add(AddingT);
                                if (AddingT.triangtes[2] != -1)
                                {
                                    _triugol T3 = (_triugol)_Triangl[AddingT.triangtes[2]];
                                    int[] nNew = GetNomers(AddingT.index, T3.index);
                                    T3.triangtes[nNew[1]] = AddingT.index;
                                }
                                Our.node[ver] = point;
                                Our.triangtes[Sl] = AddingT.index;
                            }

                            //Flip(AddingT.index, AddingT.triangtes[0]);
                            Flip(AddingT.index, AddingT.triangtes[1]);
                            Flip(AddingT.index, AddingT.triangtes[2]);
                            Flip(Our.index, Our.triangtes[0]);
                            Flip(Our.index, Our.triangtes[1]);
                            Flip(Our.index, Our.triangtes[2]);

                            break;
                        }
                }
               /* oldPoint = point;
                oldTr = nTr;
                oldC = Column;
                oldR = Row;*/
                // Point is added
                N++;
                if (N == SoC)
                {
                    CASH = CreateDinamicCAHS(CASH, m);
                    m *= 2;
                    SoC = m * m * R;
                }
            }
        }
        private int[] WalkToDarkOfMind(int nomer, _point P)
        {
            _triugol cTr = (_triugol)_Triangl[nomer];
            _point p0 = cTr.node[0];
            _point p1 = cTr.node[1];
            _point p2 = cTr.node[2];
            double Epss = 0.000000001;

            if (P != p0 & P != p1 & P != p2)
            {
                int ver = 2;
                double sin2 = SIN(P, p0, p1);
                //double sin1 = SIN(p0, p1, p2);
                //<= Epss
                if (Math.Abs(sin2) <= Epss) { return new int[] { 3, 0,cTr.index }; }
                else
                    if (sin2 < 0) { return new int[] { 1, cTr.triangtes[ver] }; }
                    else
                    {
                        ver = 0;
                        sin2 = SIN(P, p1, p2);
                        if (Math.Abs(sin2) <= Epss) {  return new int[] { 3, 1, cTr.index }; }
                        else
                            if (sin2 < 0) { return new int[] { 1, cTr.triangtes[ver] }; }
                            else
                            {
                                ver = 1;
                                sin2 = SIN(P, p2, p0);
                                if (Math.Abs(sin2) <= Epss) {  return new int[] { 3, 2, cTr.index }; }
                                else
                                    if (sin2 < 0) {  return new int[] { 1, cTr.triangtes[ver] }; }
                                    else
                                        return new int[] { 0, cTr.index };
                            }
                    }
            }
            else return new int[] { 2, cTr.index };
        }
        private double SIN(_point p0, _point p1, _point p2)
        {
            return (p0.x - p1.x) * (p0.y - p2.y) - (p0.x - p2.x) * (p0.y - p1.y);
        }
        private int[,] CreateDinamicCAHS(int[,] oldCASH, int oldM)
        {
            int[,] nCASH = new int[oldM * 2, oldM * 2];
            for (int i = 0; i < oldM; i++)
                for (int j = 0; j < oldM; j++)

                    nCASH[2 * i, 2 * j] = nCASH[2 * i, 2 * j + 1] =
                    nCASH[2 * i + 1, 2 * j] = nCASH[2 * i + 1, 2 * j + 1] = oldCASH[i, j];

            return nCASH;
        }
        private void Gan(int r)
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
        private void GanButton_Click(object sender, EventArgs e)
        {
            Gan(0);
        }
        // helpers
        private int _mod(int v)
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
        private int[] GetNomers(int f, int s)
        {
            // ѕолучаем номера противоположныж вершин смежных треугольников
            // f - номер 1 треугольника
            // s - номер 2 треугольника
            // v1 - номер вершины в 1 треугольнике           
            // v2 - номер вершины в 2 треугольнике

            _triugol First = (_triugol)_Triangl[f];
            _triugol Second = (_triugol)_Triangl[s];
            int[] result = new int[2];
            _point p1_0, p1_1, p1_2;
            _point p2_0, p2_1, p2_2;

            p1_0 = First.node[0];
            p1_1 = First.node[1];
            p1_2 = First.node[2];

            p2_0 = Second.node[0];
            p2_1 = Second.node[1];
            p2_2 = Second.node[2];

            _point p0 = p2_2;
            int i = 2;
            while (p0 == p1_0 | p0 == p1_1 | p0 == p1_2) { i--; p0 = Second.node[i]; }
            result[1] = i;

            p0 = p1_2;
            i = 2;
            while (p0 == p2_0 | p0 == p2_1 | p0 == p2_2) { i--; p0 = First.node[i]; }
            result[0] = i;

            //return new int[] {v1,v2};
            return result;
        }
        private bool _Nvertex(int _index, int v, int N)
        {
            // _inedx - номер текущего треугольника
            // v - номер текущей провер€емой вершины
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

                _triugol Neig = (_triugol)_Triangl[N];
                _triugol cTr = (_triugol)_Triangl[_index];
                int i = 0;
                while ((Neig.node[i] == cTr.node[s1]) |
                        (Neig.node[i] == cTr.node[s2])) { i++; }
                if (Neig.triangtes[i] != _index) error = true;
            }
            return error;
        }
        private void DebugFunction()
        {
            bool ERROR, error1, error2, error3;
            ERROR = error3 = error1 = error2 = false;
            for (int i = 0; i < _Triangl.Count; i++)
            {
                _triugol ctr = (_triugol)_Triangl[i];
                error1 = _Nvertex(i, 0, ctr.triangtes[0]);
                error2 = _Nvertex(i, 1, ctr.triangtes[1]);
                error3 = _Nvertex(i, 2, ctr.triangtes[2]);
                ERROR = ERROR | error1 | error2 | error3;
            }
            if (ERROR)
            {
                MessageBox.Show("Error. Ќеправильные соседи");
            }

            for (int i = 0; i < _Triangl.Count; i++)
            {
                _triugol work = (_triugol)_Triangl[i];

                if (Flip(work.index, work.triangtes[0])) MessageBox.Show("0");

                if (Flip(work.index, work.triangtes[1])) MessageBox.Show("1");

                if (Flip(work.index, work.triangtes[2])) MessageBox.Show("2");
            }
        }
        //----------IZOLINE
        private ArrayList Metka_of_Hlayer(float h)
        {
            int onepoint = 0;
            ArrayList izoLine = new ArrayList();
            _triugol t = new _triugol();
            double z1, z2, z3;
            for (int i = 0; i < _Triangl.Count; i++)
            {
                t = (_triugol)_Triangl[i];
                z1 = t.node[0].Z;
                z2 = t.node[1].Z;
                z3 = t.node[2].Z;
                double min = Math.Min(Math.Min(z1, z2), z3);
                double max = Math.Max(Math.Max(z1, z2), z3);
                if (min < h & h < max)
                {
                   
                    t.flagC = h;
                    if ((z1 <= h & h < z2) | (z2 < h & h <= z1)) { onepoint++; izoLine.Add(GetIzoPoint(t.node[1], t.node[0], h)); }
                    if ((z2 <= h & h < z3) | (z3 < h & h <= z2)) { onepoint++; izoLine.Add(GetIzoPoint(t.node[2], t.node[1], h)); }
                    if ((z3 <= h & h < z1) | (z1 < h & h <= z3)) { onepoint++; izoLine.Add(GetIzoPoint(t.node[0], t.node[2], h)); }
                   
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
                t = (_triugol)_Triangl[i];
                x = t.node[0].x / _Width;
                y = t.node[0].y / _Width;
                _color = map.GetPixel((int)(x * 99), (int)(y * 99));
                z = (_color.R + _color.G + _color.B) / k;
                t.node[0].Z = z;
                vertex[i * 3].Position = new Vector3((float)x, (float)y, (float)z);

                x = t.node[1].x / _Width;
                y = t.node[1].y / _Width;
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
        private void SetupCamera()
        {
            device.Transform.Projection = Matrix.PerspectiveFovLH((float)Math.PI / 4, this.Width / this.Height, 0.0f, 50.0f);
            device.Transform.View = Matrix.LookAtLH(new Vector3(0, 0, 3f), new Vector3(), new Vector3(0, 1, 0));
            device.Transform.World = Matrix.RotationYawPitchRoll(rx,ry,0) *Matrix.Translation(mx, my, mz);
            device.RenderState.Lighting = false;
        }
        private void MainCanvas_Paint(object sender, PaintEventArgs e)
        {
            // if (drawing) DrawIT();
        }
        private void MainCanvas_DoubleClick(object sender, EventArgs e)
        {
            
           // DrawIT();
        }
        private void button1_Click(object sender, EventArgs e)
        {
            this.Dispose();
        }

        private void MainCanvas_MouseDown(object sender, MouseEventArgs e)
        {
            mDown = true;
            iPoint = new Point(e.X, e.Y);
        }

        private void MainCanvas_MouseUp(object sender, MouseEventArgs e)
        {
            mDown = false;
        }

        private void MainCanvas_MouseMove(object sender, MouseEventArgs e)
        {
            if (mDown)
            {
                if (e.Button == MouseButtons.Left)
                {
                    my += Math.Sign(-e.Y + iPoint.Y) * 0.04f;
                    mx += Math.Sign(-e.X + iPoint.X) * 0.04f;
                    iPoint = new Point(e.X, e.Y);
                    DrawIT();
                }
                else
                    if (e.Button == MouseButtons.Right)
                    {
                        ry += Math.Sign(-e.Y + iPoint.Y) * 0.1f;
                        rx += Math.Sign(e.X - iPoint.X) * 0.1f;
                        iPoint = new Point(e.X, e.Y);
                        DrawIT();
                    }
                    else
                        if (e.Button == (MouseButtons.Left | MouseButtons.Right))
                        {
                           mz += Math.Sign(-e.Y + iPoint.Y) * 0.1f;
                            //rz += Math.Sign(e.X - iPoint.X) * 0.1f;
                            iPoint = new Point(e.X, e.Y);
                            DrawIT();
                        }
                
                }
        }

        private void MainCanvas_PreviewKeyDown(object sender, PreviewKeyDownEventArgs e)
        {
            MessageBox.Show("privet ");
        }

        private void button2_Click(object sender, EventArgs e)
        {
            colorDialog1.ShowDialog();
            izocolor = colorDialog1.Color;
        }

        
        //------------------------------------------------------------------
    }
