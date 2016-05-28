angular.module("traffic3App")
  .controller("statusCtrl", function ($scope, $http, $location, $interval) {
    $scope.data = {};

    $scope.updateStatus = function() {
      $http.get("/status").success(function (data) {
        $scope.data = data;
      }).error(function (error) {
        $scope.error = error;
      });
    };

    $scope.updateStatus();

    $scope.interval = $interval(function() {
      $scope.updateStatus();
      console.log("UpdateStatus");
    }, 5000);

  });
