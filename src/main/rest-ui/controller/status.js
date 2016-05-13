angular.module("traffic3App")
  .controller("statusCtrl", function ($scope, $http, $location) {
    $scope.data = {};

    $http.get("/status").success(function (data) {
      $scope.data = data;
    }).error(function (error) {
      $scope.error = error;
    });
  });
