angular.module('batyrd', []).
    config(['$routeProvider', function($routeProvider) {
        $routeProvider.
        when('/status', {templateUrl: 'partials/status.html', controller: StatusCtrl}).
        when('/about', {templateUrl: 'partials/about.html', controller: AboutCtrl}).
        when('/jobqueue', {templateUrl: 'partials/jobqueue.html', controller: JobsCtrl}).
        when('/layers', {templateUrl: 'partials/layers.html', controller: LayersCtrl}).
        otherwise({redirectTo: '/status'});
    }]).
    filter('fromNow', function() {
        // return a natural language string describing the
        // timestamp relative to the current time.
        // Input is expected to be a string in ISO-8601 format.
        return function(date) {
            if (!date) {
                return "";
            }
            return moment(date).fromNow();
        }
    }).
    filter('secondDuration', function() {
        // display the duration as a human-compatible text. input is expected
        // to be in seconds
        return function(secs) {
            if (!secs) { // fallback, as angular initalizes the view with undefined,
                         // what moment.js does not especially like.
                return "? minutes";
            }
            return moment.duration(secs, 'seconds').humanize();
        }
    });
