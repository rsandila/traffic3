angular.module("traffic3App")
  .controller("serverCtrl", function ($scope, $http, $location) {
    $scope.data = {};
    $scope.addActive = 0;
    $scope.protocols = {};
    $scope.contentmanagers = {};
    $scope.add = {
      id: 0,
      host: "::",
      port: 10000,
      min: 100,
      max: 1000
    };
    $http.get("/status/supports/protocol").success(function (data) {
      $scope.protocols = data;
    }).error(function (error) {
      $scope.error = error;
    });

    $http.get("/status/supports/contentmanager").success(function (data) {
      $scope.contentmanagers = data;
    }).error(function (error) {
      $scope.error = error;
    });

    $http.get("/server").success(function (data) {
      $scope.data = data;
    }).error(function (error) {
      $scope.error = error;
    });

    $scope.showAdd = function() {
      $scope.add.success = null;
      $scope.add.error = null;
      $scope.addActive = 1;
    }

    $scope.isAddActive = function() {
      return $scope.addActive == 1;
    }

    $scope.addSubmit = function() {
      $http.put("/server?id=" + $scope.add.id +
                        "&protocol=" + $scope.add.protocol +
                        "&contentmanager=" + $scope.add.contentmanager +
                        "&host=" + $scope.add.host +
                        "&port=" + $scope.add.port +
                        "&min=" + $scope.add.min +
                        "&max=" + $scope.add.max
                      ).success(function (data) {
                        $scope.add.success = data
                      }).error(function (error) {
                        $scope.add.error = error;
                      });
      $scope.addActive = 0;
    }
  });
