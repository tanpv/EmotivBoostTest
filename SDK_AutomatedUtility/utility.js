/*jslint white: true, node: true, unparam: true, sloppy: true, bitwise: true,eqeq: true*/
var config = require('./config.json');
var request = require('request');
var fs = require('fs');
var xml2js = require('xml2js');
var glob = require("glob");
var i = 0,
    tempResult = [];

/* This function is just used to create the object for the Test Run creation.
In this function we are setting the status of Test Case also, that will reflect on a Test Run.
Also you can get the TestRun object that will be passed in createTestRun() method, just by uncommenting console.log().*/
function createTestRunObject(data, jsonResult, callback) {
    var createTestRunData = {},
        items = [],
        i = 0,
        j = 0,
        testname = [],
        testCaseName = '',
        storyId = '';
    createTestRunData.projectKey = process.argv[3];
    createTestRunData.name = "Build_" + process.argv[2];
    for (j = 0; j < jsonResult.length; j = j + 1) {
        if (process.argv[3] === "CLO") {
            for (i = 0; i < data.length; i = i + 1) {
                testCaseName = jsonResult[j].classname + "." + jsonResult[j].name;
                if (testCaseName === data[i].name) {
                    items.push({
                        'testCaseKey': data[i].key,
                        'userKey': data[i].owner || config.owner,
                        'status': jsonResult[j].failure ? "Fail" : "Pass"
                    });
                    break;
                }
            }
        } else {
            for (i = 0; i < data.length; i = i + 1) {
                testname = jsonResult[j].name.split('_', 2);
                storyId = testname[0] + "-" + testname[1];
                testCaseName = jsonResult[j].name.slice(storyId.length + 1);
                if (testCaseName === data[i].name) {
                    items.push({
                        'testCaseKey': data[i].key,
                        'userKey': data[i].owner || config.owner,
                        'status': jsonResult[j].Error ? "Fail" : "Pass"
                    });
                    break;
                }
            }
        }
    }
    createTestRunData.items = items;
    //console.log(createTestRunData);
    callback(createTestRunData);
}

/* This function is used to check whether a particular name of test case already exists or not. */
function findTestCase(o, value) {
    //console.log("\n" + JSON.stringify(o) + "\n");
    var i;
    for (i = 0; i < o.length; i = i + 1) {
        if (o[i].name === value) {
            return true;
        }
    }
    return false;
}

/* This function is used to the POST request for the Test Case. In response we get the key of the Test Case created.
Also you can check the Key for each Test Case, just by uncommenting console.log(). */
function postTestCaseRequest(obj, result) {
    var auth = "Basic " + new Buffer(config.username + ":" + config.password).toString("base64"),
        mainurl = config.base_url + "/rest/kanoahtests/latest/testcase";
    request({
            method: "POST",
            json: true,
            url: mainurl,
            headers: {
                "Authorization": auth
            },
            body: obj
        },
        function (error, response, body) {
            //console.log("\n" + JSON.stringify(body) + "\n");
            if (response.statusCode === 201) {
                result.key = body.key;
            } else {
                console.log(body);
                process.exit(0);
            }
        });
}

/* This function is used to create the Test Cases. If not there in the list, it will create one. */
function createTestCases(result, allTestCasesResult, callback) {
    var checkValue = false,
        testCaseName = [],
        issueLinkArray = '',
        obj = {},
        i = 0,
        maxlength = result.length,
        finalTestCaseName = '',
        storyId = '';
    while (i < maxlength) {
        issueLinkArray = [];
        if (process.argv[3] === "CLO") {
            finalTestCaseName = result[i].classname + "." + result[i].name;
            checkValue = findTestCase(allTestCasesResult, finalTestCaseName);
            if (result[i].properties) {
                issueLinkArray = result[i].properties.property.value.split(', ');
            }
        } else {
            testCaseName = result[i].name.split('_', 2);
            storyId = testCaseName[0] + "-" + testCaseName[1];
            finalTestCaseName = result[i].name.slice(storyId.length + 1);
            checkValue = findTestCase(allTestCasesResult, finalTestCaseName);
            issueLinkArray.push(storyId);
        }
        if (!checkValue) {
            obj = {
                "projectKey": process.argv[3],
                "name": finalTestCaseName,
                "status": "Approved",
                "owner": config.owner,
                "labels": ["Automated"],
                "issueLinks": issueLinkArray
            };
            postTestCaseRequest(obj, result[i]);
        }
        i = i + 1;
    }
    if (i === maxlength) {
        setTimeout(function () {
            callback(result);
        }, 5000);
    }
}

/* This function is used to get all the Test Cases of a particular project whose labels are Automated.
Also you can check the response, just by uncommenting console.log(). */
function getAutomatedTestCases(callback) {
    var auth = "Basic " + new Buffer(config.username + ":" + config.password).toString("base64"),
        mainurl = config.base_url + "/rest/kanoahtests/latest/testcase/search",
        queryParams = {},
        length = 0,
        k = 0,
        j = 0;
    queryParams = {
        'query': "projectKey = " + process.argv[3] + " AND labels IN (\"Automated\") ",
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
            if (body.length == 200) {
                for (j = i; j < length; j = j + 1) {
                    tempResult[j] = body[k];
                    k = k + 1;
                }
                i = i + 200;
                getAutomatedTestCases(callback);
            } else if (response.statusCode === 200 && body.length < 200) {
                for (j = i; j < length; j = j + 1) {
                    tempResult[j] = body[k];
                    k = k + 1;
                }
                callback(tempResult);
            } else {
                console.log(body);
                process.exit(0);

            }
        });
}

/* This function is used to create Test Run for all the Test Cases.
In response it will return the key of Test Run created. */
function createTestRun(data, callback) {
    var auth = "Basic " + new Buffer(config.username + ":" + config.password).toString("base64"),
        mainurl = config.base_url + "/rest/kanoahtests/latest/testrun";
    request({
            method: "POST",
            json: true,
            url: mainurl,
            headers: {
                "Authorization": auth
            },
            body: data
        },
        function (error, response, body) {
            //console.log(body);
            if (response.statusCode === 201) {
                callback(body);
            } else {
                console.log(body);
                process.exit(0);
            }
        });
}

/* This function is used to read the data of XML file based on project and parse it.
After parsing it passes the data to createTestCases method for creating test cases. */
function readXMLFile(allTestCasesResult, fileName, callback) {
    var parser = new xml2js.Parser({
            explicitArray: false,
            mergeAttrs: true,
            charkey: 'errorText',
            attrkey: ''
        }),
        jsonResult = [];
    fs.readFile(fileName, 'utf8', function (err, data) {
        if (data) {
            parser.parseString(data, function (err, result) {
                if (result) {
                    if (process.argv[3] === "CLO") {
                        jsonResult = result.testsuite.testcase;
                    } else {
                        jsonResult = result.TestLog.TestSuite.TestSuite.TestCase;
                    }
                    createTestCases(jsonResult, allTestCasesResult, function (result) {

                        getAutomatedTestCases(function (response) {

                            createTestRunObject(response, result, function (createTestRunData) {

                                createTestRun(createTestRunData, function (finalResult) {
                                    callback(finalResult);
                                });
                            });
                        });
                    });
                } else {
                    console.log(err);
                    process.exit(0);
                }
            });
        } else {
            console.log(err);
            process.exit(0);
        }
    });
}

/* This function gets the xml file from source folder */
function getXMLFile(allTestCasesResult, callback) {
    var fileName = '',
        location = '';
    if (process.argv[3] === "CLO") {
        fileName = 'out.xml';
        readXMLFile(allTestCasesResult, fileName, callback);
    } else {
        location = config.xml_location + "result*.xml";
        console.log("Location :" + location);
        glob(location, function (er, files) {
            fileName = files[0];
            console.log("\nFileName : " + fileName);
            readXMLFile(allTestCasesResult, fileName, callback);
        });
    }
}


var utility = {
    'getXMLFile': getXMLFile
};

module.exports = utility;