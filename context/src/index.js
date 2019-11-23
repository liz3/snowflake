const express = require("express");
const net = require("net");
const spawn = require("child_process").spawn;
const server = express();

let generator = spawn("./snowflake", ["1", "1"]);
generator.stderr.pipe(process.stderr);
generator.on("error", () => {
  console.log("proc_err");
  generator.kill();
  generator = spawn("./snowflake", ["1", "1"]);
  generator.stderr.pipe(process.stderr);
});
generator.on("exit", () => {
  console.log("proc_exit");
  generator.kill();
  generator = spawn("./snowflake", ["1", "1"]);
  generator.stderr.pipe(process.stderr);
});
const getId = () => {
  return new Promise((resolve, reject) => {
    const client = new net.Socket();
    client.on("data", function(data) {
      client.write("close");
      try {
        const parsed = JSON.parse(data);
        resolve({ ...parsed, message: "success" });
      } catch (err) {
        reject({ error: "internal_failure" });
      }
    });
    client.connect(8888, "localhost", () => {
      client.write("id");
    });
  });
};
const parseId = orig_id => {
  return new Promise((resolve, reject) => {
    const client = new net.Socket();
    client.on("data", function(data) {
      client.write("close");
      try {
        const parsed = JSON.parse(data);
        resolve(parsed);
      } catch (err) {
        reject({ error: "internal_failure" });
      }
    });
    client.connect(8888, "localhost", () => {
      client.write(orig_id);
    });
  });
};

server.get("/api/id", async (req, res) => {
  try {
    const idData = await getId();
    res.json(idData);
  } catch (err) {
    res.status(500).json(err);
  }
});
server.get("/api/parse/:id", async (req, res) => {
  const id = req.params.id;
  if (typeof id !== "string" || id.length < 5 || id.length > 25) {
    res.status(401).json({ error: "mallformed_request" });
  } else {
    try {
      const { timestamp, worker_id, region_id, increment } = await parseId(id);
      const time = new Date(timestamp * 1000);
      res.json({
        success: true,
        parsed_time: time,
        timestamp,
        worker_id,
        region_id,
        increment
      });
    } catch (err) {
      res.status(500).json(err);
    }
  }
});
server.get("/", async (req, res) => {
  try {
    const idData = await getId();
    res.set("Content-type", "text/html; charset=utf8");
    res.send(`<!DOCTYPE html>
    <html>
    <head>
    <meta property="og:type" content="website" />
    <meta property="og:tite" content="${idData.id} | Illyra Snowflake" />
    <meta property="og:description" content="${idData.id}" />
    <meta property="og:url" content="https://id.illyria.io/from/${idData.id}" />
    <title>${idData.id} | Illyra Snowflake</title>
    </head>
    <body>
    <h2>Your id is:</h2>
    <p>${idData.id}<p>
    </body>
    </html>`);
  } catch (err) {
    res.set("Content-type", "text/html; charset=utf8");

    res.status(500).res.send(`<!DOCTYPE html>
    <html>
    <head>
    <meta property="og:type" content="website" />
    <meta property="og:url" content="https://id.illyria.io/" />
    <title>Error | Illyra Snowflake</title>
    </head>
    <body>
    <p>${error}<p>
    </body>
    </html>`);
  }
});
server.get("/from/:id", async (req, res) => {
    const id = req.params.id;
    if (typeof id !== "string" || id.length < 5 || id.length > 25) {
      res.status(401).json({ error: "mallformed_request" });
    } else {
      try {
        const { timestamp, worker_id, region_id, increment } = await parseId(id);
        const time = new Date(timestamp * 1000);
        res.set("Content-type", "text/html; charset=utf8");
        res.send(`<!DOCTYPE html>
        <html>
        <head>
        <meta property="og:type" content="website" />
        <meta property="og:tite" content="${id} Reversed | Illyra Snowflake" />
        <meta property="og:description" content="Time: ${timestamp} OR ${time}\nRegion/DatacenterId: ${region_id}\nWorker Id: ${worker_id}\nIncrement: ${increment}" />
        <meta property="og:url" content="https://id.illyria.io/from/${id}" />
        <title>${id} Reversed | Illyra Snowflake</title>
        <style>
        p {
            margin: 2px 0;            
        }
        </style>
        </head>
        <body>
        <h2>Info:</h2>
        <p>Given snowflake: ${id}<p>
        <p>Time: ${timestamp} OR ${time}<p>
        <p>Region/Datacenter identifier: ${region_id}<p>
        <p>Worker Indentifier: ${worker_id}<p>
        <p>Worker increment in time of generation: ${increment}<p>
        </body>
        </html>`);
      } catch (err) {
        res.status(500).res.send(`<!DOCTYPE html>
    <html>
    <head>
    <meta property="og:type" content="website" />
    <meta property="og:url" content="https://id.illyria.io/" />
    <title>Error | Illyra Snowflake Reverser</title>
    </head>
    <body>
    <p>${error}<p>
    </body>
    </html>`);
      }
    }
  });
server.listen(8080);
