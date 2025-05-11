navigator.geolocation.getCurrentPosition(
  (pos) => {
    alert( pos.coords.latitude + ' ' +  pos.coords.longitude);
  },
  (err) => {
    alert(err.message);
  }
);
