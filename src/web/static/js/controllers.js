/**
 * items to list in the menu
 */
function MenuListCtrl($scope) {
    $scope.menuitems = [
    {
        "name": "Status",
        "link": "status"
    },
    {
        "name": "Layers",
        "link": "layers"
    },
    {
        "name": "Jobqueue",
        "link": "jobqueue"
    }
    ];
};


function StatusCtrl($scope, $http, $timeout) {

    // update interval of in seconds
    $scope.updateIntervalSecs = 5;

    var fetchStatus = function() {
        $http.get('api/status.json').success(function(data) {
            $scope.status = data;
        });
    }
    fetchStatus();

    // periodic reload of the views data
    // this is ugly and the stack grows and grows
    // -- replace with something better
    var cancelRefresh;
    var intervalFetch = function() {
        cancelRefresh = $timeout(function performFetch() {
            fetchStatus()
            cancelRefresh = $timeout(intervalFetch, $scope.updateIntervalSecs * 1000);
        }, $scope.updateIntervalSecs * 1000);
    };
    intervalFetch();

    // destroy the timeout when the view is detroyed
    $scope.$on('$destroy', function(e) {
                $timeout.cancel(cancelRefresh);
    });
}

function AboutCtrl($scope, $http) {
    var fetchStatus = function() {
        $http.get('api/status.json').success(function(data) {
            $scope.status = data;
        });
    }
    fetchStatus();
}

function LayersCtrl($scope, $http) {
    $scope.layerlist = {};
    var fetchLayers = function() {
        $http.get('api/layers.json').success(function(data) {
            $scope.layerlist = data;
        });
    }
    fetchLayers();

    // TODO: add helplink to http://www.gdal.org/ogr/ogr_sql.html
    $scope.filterHelp = function() {
        if (console) {
            console.log("Filter help");
        }
    };

    $scope.pullLayer = function(layer) {
        if (console) {
            console.log(layer);
        }
        var job = {
            layerName:  layer.name,
            filter:     layer.filter
        };
        var promise = $http.post('api/create', job);
    }
}

function JobsCtrl($scope, $http) {
    $scope.joblist = {};
    var fetchJobs = function() {
        $http.get('api/jobs.json').success(function(data) {
            $scope.joblist = data;
        });
    }
    fetchJobs();

    // refresh button
    $scope.refreshJobs = function() {
        fetchJobs();
    };
}

