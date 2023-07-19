setInterval(function () {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      var response = this.responseText;
      response = response.split(" ");
      document.getElementById("temperature").innerHTML = response[0];
      document.getElementById("humidity").innerHTML = response[1];
      document.getElementById("AQI").innerHTML = response[2];
      document.getElementById("FanStatus").innerHTML = response[3];
      document.getElementById("threshTemp").innerHTML = response[4];
      document.getElementById("threshHumidity").innerHTML = response[5];
      document.getElementById("threshCO").innerHTML = response[6];
    }
  };
  xhttp.open("GET", "/Data", true);
  xhttp.send();
}, 5000);

var xhttp1 = new XMLHttpRequest();
xhttp1.onreadystatechange = function () {
  if (this.readyState == 4 && this.status == 200) {
    document.body.innerHTML = this.responseText;
  }
};
xhttp1.open(
  "GET",
  "https://a_mubin.gitlab.io/webdev/embeddedProject/body.htm",
  true
);
xhttp1.send();

function formSubmit() {
  var x = document.getElementById("frm1");
  var text = "/get?";
  var i;
  for (i = 0; i < x.length; i++) {
    if (Number.isFinite(x.elements[i].value)) {
      text += x.elements[i].name + "=" + x.elements[i].value;
    }
  }
  var xhttp2 = new XMLHttpRequest();
  xhttp2.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("formSubmit").innerHTML = this.responseText;
    }
  };
  xhttp2.open("GET", text, true);
  xhttp2.send();
}
