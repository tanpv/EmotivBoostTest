/*jslint white: true, node: true, unparam: true, sloppy: true*/
var express = require('express');
var bodyParser = require("body-parser");
var config = require('./config.json');
var request = require('request');
var app = express();
var utility = require('./utility.js');
app.set('json spaces', 40);
app.set('view options', {
    pretty: true
});
app.locals.pretty = true;
app.use(bodyParser.urlencoded({
    extended: false
}));
app.use(bodyParser.json());
var i = 0,
    tempResult = [];

/* This function is used to get all the test cases of a particular project.
Only projectKey is required to be passed as query parameters */
function getAllTestCases(callback) {
    var auth = "Basic " + new Buffer(config.username + ":" + config.password)
        .toString("base64"),
        mainurl = config.base_url + "/rest/kanoahtests/latest/testcase/search",
        queryParams = {},
        length = 0,
        k = 0,
        j = 0;
    queryParams = {
        'query': "projectKey = " + process.argv[3],
        'startAt': i
    };
    request({
            method: "GET",
            json: true,
            url: mainurl,
            headers: {
                "Authorization": auth
            },
            qs: queryParams
        },
        function (error, response, body) {
            length = i + body.length;
            //console.log(body);
            if (body.length === 200) {
                for (j = i; j < length; j = j + 1) {
                    tempResult[j] = body[k];
                    k = k + 1;
                }
                i = i + 200;
                getAllTestCases(callback);
            } else if (response.statusCode === 200 && body.length < 200) {
                for (j = i; j < length; j = j + 1) {
                    tempResult[j] = body[k];
                    k = k + 1;
                }
                utility.getXMLFile(tempResult, function (response) {
                    callback(response);
                });
            } else {
                console.log(body);
                process.exit(1);

            }
        });

}

app.listen(4000, function () {
    console.log('Utility listening on jira.emotivcloud.com!');
    getAllTestCases(function (response) {
        console.log("\nThe Test Run ID is: " + JSON.stringify(response));
        process.exit(0);
    });
});