const express = require("express");
const admin = require("firebase-admin");
const serviceAccount = require("./key.json");

const app = express();

// fire base init ---------
admin.initializeApp({
  credential: admin.credential.cert(serviceAccount),
  databaseURL: "https://powersync-f77b7-default-rtdb.firebaseio.com" 
});

const db = admin.database();

app.use(express.json()); // Allows the server to read JSON sent by your app
app.use(express.urlencoded({ extended: true }));

// Log all incoming requests for debugging
app.use((req, res, next) => {
  console.log(`${req.method} request to ${req.url}`);
  next();
});

//  Commands Listener
let latestCommands = {};
db.ref("commands").on("value", (snapshot) => {
  latestCommands = snapshot.val() || {};
  console.log("Updated commands from Firebase:", latestCommands);
}, (error) => {
  console.error("Firebase read error:", error.message);
});

app.get("/", (req, res) => {
  res.send("Server is alive! Firebase is linked.");
});

app.get("/api/commands", (req, res) => {
  res.json(latestCommands);
});

// POST sensor data
app.post("/api/data", (req, res) => {
  const sensorData = req.body;
  console.log("Received from device:", sensorData);

  // Push to Firebase
  db.ref("sensors").push(sensorData)
    .then(() => {
      res.status(200).send("Data stored in Firebase");
    })
    .catch((err) => {
      console.error("Firebase push error:", err);
      res.status(500).send("Error saving to Firebase");
    });
});

// server start
const PORT = 3000;
app.listen(PORT, () => {
  console.log(`Server running on port ${PORT}`);
});




/*const express = require("express");
const app = express();

// ----- firebase -------
/*let db = null;
try {
  const admin = require("firebase-admin");
  const serviceAccount = require("./key.json");

  admin.initializeApp({
    credential: admin.credential.cert(serviceAccount),
    databaseURL: "https://your-project.firebaseio.com"
  });

  db = admin.database();
  console.log("Firebase connected");
} catch (err) {
  console.log("Firebase NOT connected:", err.message);
} 

app.use(express.urlencoded({ extended: true }));

app.get("/", (req, res) => {
  res.send("Server is alive!\n Horyay! For real");
});


let latestData = {};

// ---- post ----- 

const admin = require("firebase-admin");
const serviceAccount = require("./key.json"); // need to download this key from firebase

admin.initializeApp({
  credential: admin.credential.cert(serviceAccount),
  databaseURL: "https://arduino-proj-7b746-default-rtdb.firebaseio.com/" // have to link to firebase url once done
});

const db = admin.database();
let latestCommands = {};

db.ref("commands").on("value", (snapshot) => {
  latestCommands = snapshot.val() || {};
  console.log("Updated commands:", latestCommands);
});

// view latest data
app.get("/api/commands", (req, res) => {
  res.json(latestCommands);
});

app.post("/api/data", (req, res) => { // for whn firebase is connected uncomment this code
  latestData = req.body;
  console.log("Received:", latestData);

  // Send to Firebase if available
  if (db) {
    db.ref("sensors").push(req.body);
  }

  res.send("OK");
}); 

app.post("/api/data", (req, res) => { // in place of the firebase code before connection
  console.log("Received:", req.body);
  res.send("OK");
}); 


// Log all requests
app.use((req, res, next) => {
  console.log(req.method, req.url);
  next();
});

app.listen(3000, () => {
  console.log("Server running on port 3000");
});
*/
