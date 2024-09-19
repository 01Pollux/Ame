
using AmeSharp.Engine;

Console.WriteLine("Hello World!");

IAmeEngine engine = new();
engine.Tick();
engine.Exit(0);
