const https = require('https');
const { JSDOM } = require("jsdom");

https.get('https://www.alibabacloud.com/help/zh/doc-detail/25378.htm?spm=a2c63.p38356.b99.5.6f1c4865fBoAHT#queue', (resp) => {
  let data = '';

  // A chunk of data has been recieved.
  resp.on('data', (chunk) => {
    data += chunk;
  });

  // The whole response has been received. Print out the result.
  resp.on('end', () => {
    const dom = new JSDOM(data);
    for (const tbody of dom.window.document.querySelectorAll("tbody")) {
      for (const tr of tbody.querySelectorAll("tr")) {
        const tds = tr.querySelectorAll("td");
        if (tds.length <= 5) {
          continue;
        }
        const name = tds[0].textContent;
        const cpus = parseInt(tds[1].textContent);
        const mem = parseFloat(tds[2].textContent);
        const bw = parseFloat(tds[4].textContent);
        const pps = parseFloat(tds[5].textContent);
        console.log(name, cpus, mem, bw, pps)

      }
    }
  });

}).on("error", (err) => {
  console.log("Error: " + err.message);
});
