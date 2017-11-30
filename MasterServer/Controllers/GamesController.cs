using Microsoft.AspNetCore.Mvc;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MasterServer.Controllers
{
    [Route("api/[controller]")]
    public class GamesController : Controller
    {
        [HttpGet]
        public string Get(string name = "Human")
        {
            return "Hello, " + name;
        }
    }
}
