using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Windows.Forms;

namespace ConvexHull
{
    class Vertex
    {
        public int _index;
        public Point _point;         
      
        public Vertex(int index, Point point)
        {
            _index = index; _point = point;
        }

    }
    class ConvexHullBuilder
    {
        //or Vertex
        public _point _leftPoint;
        public _point _rightPoint;
        //
        public ArrayList _dateArray = new ArrayList();
        public ArrayList _lowerPartition = new ArrayList();
        public ArrayList _upperPartition = new ArrayList();
        public ArrayList _lowerHull = new ArrayList();
        public ArrayList _upperHull = new ArrayList();
        public ArrayList _result_convex_hull = new ArrayList();
        public Boolean Error = true;


        public ConvexHullBuilder(ArrayList inPutDate, PictureBox Picture)
        {

            try
            {
                CHGraphics myGraphics = new CHGraphics(Picture);
                ComparerPoint MyComparePoint = new ComparerPoint();

                _dateArray = (ArrayList)inPutDate.Clone();
                _dateArray.Sort(MyComparePoint);

                _leftPoint = (_point)_dateArray[0];
                _dateArray.RemoveAt(0);
                _rightPoint = (_point)_dateArray[_dateArray.Count - 1];
                _dateArray.RemoveAt(_dateArray.Count - 1);


                _point betwPoint = new _point();
                for (int i = 0; i < _dateArray.Count; i++)
                {
                    //prbar.Value = i;
                    betwPoint = (_point)_dateArray[i];
                    double det = Determinant(_leftPoint, _rightPoint, betwPoint);
                    if (det > 0) _upperPartition.Add(betwPoint);
                    else _lowerPartition.Add(betwPoint);
                }

                //MessageBox.Show("This is partition");
                Build_HullPartition(_upperPartition, _upperHull, 1);
                Build_HullPartition(_lowerPartition, _lowerHull, -1);

                _lowerHull.Reverse();
                _upperHull.InsertRange(_upperHull.Count - 1, _lowerHull);

                _result_convex_hull = _upperHull;
                _result_convex_hull.RemoveRange(_result_convex_hull.Count - 2, 2);
                // myGraphics.DrawConvexHull(_result_convex_hull);
                myGraphics.DrawMidleLine(_leftPoint, _rightPoint);

            }
            catch (ArgumentOutOfRangeException e)
            {
                MessageBox.Show("ќшибка. ¬ведите числа");
            }//*/
        }

        double Determinant(_point left, _point right, _point between)
        {
            return (left.x - right.x) * (between.y - right.y)
                - (between.x - right.x) * (left.y - right.y);
        }
        

        void Build_HullPartition(ArrayList _input, ArrayList _outPut, int factor)
        {
            // factor: 1 for Upper, -1 for Lower;
            _input.Add(_rightPoint);
            _outPut.Add(_leftPoint);


            int pos = _input.Count;
            while (pos > 0)
            {
                _outPut.Add(_input[_input.Count - pos]);

                while (_outPut.Count >= 3)
                {

                    if (factor * Determinant((_point)_outPut[_outPut.Count - 3],
                                             (_point)_outPut[_outPut.Count - 1],
                                             (_point)_outPut[_outPut.Count - 2]) <= 0)
                    {
                        _outPut.RemoveAt(_outPut.Count - 2);

                    }
                    else break;
                }
                pos--;

            }

        }
    }
}
