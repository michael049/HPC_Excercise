using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HpcExercise.RandomListBuilder
{
    class Program
    {
        static void Main(string[] args)
        {
            int amount = 4096*2*2*2;
            long sum = 0;

            // Write the string to a file.
            System.IO.StreamWriter file = new System.IO.StreamWriter("../../../HpcExercise2/numbers.txt");

            file.WriteLine(amount);
            
            int i;
            for (i = 0; i < amount; i++)
            {
                file.WriteLine(i);
                sum += i;
            }

            file.WriteLine("// This file contains {0} numbers, sum is: {1} sum without last element: {2}", amount, sum, sum-i+1);
            file.Close();
        }
    }
}
