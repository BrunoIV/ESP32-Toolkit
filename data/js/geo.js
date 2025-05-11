navigator.geolocation.getCurrentPosition(
  (pos) => {
    fetch(window.location.origin + "/save_coords", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({
        lat: pos.coords.latitude,
        lon: pos.coords.longitude
      })
    });
  },
  (err) => {
    alert(err.message);
  }
);
