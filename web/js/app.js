document.addEventListener("DOMContentLoaded", () => {
  const login = sessionStorage.getItem("login") || "admin";
  const avatar = document.getElementById("user-avatar");
  if (avatar) avatar.textContent = login.charAt(0).toUpperCase();

  switchTab("dashboard");

  document.querySelectorAll(".nav-item").forEach((btn) => {
    btn.addEventListener("click", () => switchTab(btn.dataset.tab));
  });

  loadAll();

  document
    .getElementById("vehicle-form")
    .addEventListener("submit", onVehicleSubmit);
  document
    .getElementById("driver-form")
    .addEventListener("submit", onDriverSubmit);
  document.getElementById("trip-form").addEventListener("submit", onTripSubmit);
  document
    .getElementById("policy-form")
    .addEventListener("submit", onPolicySubmit);
  document
    .getElementById("maintenance-form")
    .addEventListener("submit", onMaintenanceSubmit);
});

function switchTab(tab) {
  document
    .querySelectorAll(".nav-item")
    .forEach((b) => b.classList.remove("active"));
  document
    .querySelectorAll(".tab-content")
    .forEach((c) => c.classList.add("hidden"));

  const btn = document.querySelector(`.nav-item[data-tab="${tab}"]`);
  if (btn) btn.classList.add("active");

  const section = document.getElementById(`tab-${tab}`);
  if (section) section.classList.remove("hidden");
}

function logout() {
  sessionStorage.clear();
  window.location.href = "index.html";
}

function notify(msg, type = "success") {
  const el = document.getElementById("notification");
  el.textContent = msg;
  el.className = "notification " + type;
  el.classList.remove("hidden");
  setTimeout(() => el.classList.add("hidden"), 3000);
}

async function loadAll() {
  await Promise.all([
    loadCounters(),
    loadVehicles(),
    loadDrivers(),
    loadTrips(),
    loadPolicies(),
    loadMaintenance(),
  ]);
}

async function loadCounters() {
  const [vehicles, drivers, policies] = await Promise.all([
    fetchJSON("/api/vehicles"),
    fetchJSON("/api/drivers"),
    fetchJSON("/api/policies/expiring"),
  ]);
  const vEl = document.getElementById("dash-vehicles-count");
  const dEl = document.getElementById("dash-drivers-count");
  const pEl = document.getElementById("dash-policies-count");
  if (vEl) vEl.textContent = `${vehicles.length} единиц`;
  if (dEl) dEl.textContent = `${drivers.length} человек`;
  if (pEl) pEl.textContent = `${policies.length} истек.`;
}

/* ===== ТС ===== */
async function loadVehicles() {
  const data = await fetchJSON("/api/vehicles");
  const container = document.getElementById("vehicles-list");
  if (!container) return;
  container.innerHTML = "";
  if (data.length === 0) {
    container.innerHTML =
      '<div style="text-align:center; color:#777; padding:20px;">Нет транспортных средств</div>';
    return;
  }
  data.forEach((v) => {
    const div = document.createElement("div");
    div.className = "list-card";
    div.innerHTML = `
            <div class="list-card-info">
                <div class="title">${v.gos_number} — ${v.brand} ${v.model}</div>
                <div class="subtitle">${v.year} · ${v.type} · ${v.mileage} км · ${v.state}</div>
            </div>
            <div class="list-card-actions">
                <button class="btn btn-edit" onclick='openVehicleForm(${JSON.stringify(v).replace(/'/g, "&#39;")})'>Изменить</button>
                <button class="btn btn-delete" onclick="deleteVehicle(${v.id}, '${v.gos_number}')">Удалить</button>
            </div>
        `;
    container.appendChild(div);
  });
}

function openVehicleForm(vehicle) {
  const form = document.getElementById("vehicle-form");
  const title = document.getElementById("vehicle-modal-title");
  form.reset();
  if (vehicle) {
    title.textContent = "Изменение ТС";
    form.id.value = vehicle.id;
    form.gos_number.value = vehicle.gos_number;
    form.brand.value = vehicle.brand;
    form.model.value = vehicle.model;
    form.year.value = vehicle.year;
    form.vin.value = vehicle.vin;
    form.type.value = vehicle.type;
    form.mileage.value = vehicle.mileage;
    form.state.value = vehicle.state;
    form.driver_id.value = vehicle.driver_id;
  } else {
    title.textContent = "Добавление ТС";
    form.id.value = 0;
  }
  document.getElementById("vehicle-modal").classList.remove("hidden");
}
function closeVehicleForm() {
  document.getElementById("vehicle-modal").classList.add("hidden");
}

async function onVehicleSubmit(e) {
  e.preventDefault();
  const fd = new FormData(e.target);
  const params = new URLSearchParams();
  for (const [k, v] of fd.entries()) params.append(k, v);
  const isEdit = fd.get("id") && fd.get("id") !== "0";
  const url = isEdit ? "/api/vehicles/update" : "/api/vehicles";
  try {
    const r = await fetch(url, {
      method: "POST",
      headers: { "Content-Type": "application/x-www-form-urlencoded" },
      body: params.toString(),
    });
    const data = await r.json();
    if (data.success) {
      closeVehicleForm();
      notify(isEdit ? "ТС обновлено" : "ТС добавлено");
      loadVehicles();
      loadCounters();
    } else notify(data.message || "Ошибка", "error");
  } catch (err) {
    notify("Сервер недоступен", "error");
  }
}

async function deleteVehicle(id, gos_number) {
  if (!confirm(`Удалить ТС ${gos_number}?`)) return;
  const params = new URLSearchParams();
  params.append("id", id);
  try {
    const r = await fetch("/api/vehicles/delete", {
      method: "POST",
      headers: { "Content-Type": "application/x-www-form-urlencoded" },
      body: params.toString(),
    });
    const data = await r.json();
    if (data.success) {
      notify("ТС удалено");
      loadVehicles();
      loadCounters();
    } else notify(data.message || "Ошибка", "error");
  } catch (err) {
    notify("Сервер недоступен", "error");
  }
}

/* ===== Водители ===== */
async function loadDrivers() {
  const data = await fetchJSON("/api/drivers");
  const container = document.getElementById("drivers-list");
  if (!container) return;
  container.innerHTML = "";
  if (data.length === 0) {
    container.innerHTML =
      '<div style="text-align:center; color:#777; padding:20px;">Нет водителей</div>';
    return;
  }
  data.forEach((d) => {
    const div = document.createElement("div");
    div.className = "list-card";
    div.innerHTML = `
            <div class="list-card-info">
                <div class="title">${d.fio}</div>
                <div class="subtitle">Права: ${d.license_number} · стаж ${d.experience} лет · кат. ${d.categories} · до ${d.license_valid_until}</div>
            </div>
            <div class="list-card-actions">
                <button class="btn btn-edit" onclick='openDriverForm(${JSON.stringify(d).replace(/'/g, "&#39;")})'>Изменить</button>
                <button class="btn btn-delete" onclick="deleteDriver(${d.id}, '${d.fio.replace(/'/g, "&#39;")}')">Удалить</button>
            </div>
        `;
    container.appendChild(div);
  });
}

function openDriverForm(driver) {
  const form = document.getElementById("driver-form");
  const title = document.getElementById("driver-modal-title");
  form.reset();
  if (driver) {
    title.textContent = "Изменение водителя";
    form.id.value = driver.id;
    form.fio.value = driver.fio;
    form.license_number.value = driver.license_number;
    form.experience.value = driver.experience;
    form.categories.value = driver.categories;
    form.license_valid_until.value = driver.license_valid_until;
  } else {
    title.textContent = "Добавление водителя";
    form.id.value = 0;
  }
  document.getElementById("driver-modal").classList.remove("hidden");
}
function closeDriverForm() {
  document.getElementById("driver-modal").classList.add("hidden");
}

async function onDriverSubmit(e) {
  e.preventDefault();
  const fd = new FormData(e.target);
  const params = new URLSearchParams();
  for (const [k, v] of fd.entries()) params.append(k, v);
  const isEdit = fd.get("id") && fd.get("id") !== "0";
  const url = isEdit ? "/api/drivers/update" : "/api/drivers";
  try {
    const r = await fetch(url, {
      method: "POST",
      headers: { "Content-Type": "application/x-www-form-urlencoded" },
      body: params.toString(),
    });
    const data = await r.json();
    if (data.success) {
      closeDriverForm();
      notify(isEdit ? "Водитель обновлён" : "Водитель добавлен");
      loadDrivers();
      loadCounters();
    } else notify(data.message || "Ошибка", "error");
  } catch (err) {
    notify("Сервер недоступен", "error");
  }
}

async function deleteDriver(id, fio) {
  if (!confirm(`Удалить водителя ${fio}?`)) return;
  const params = new URLSearchParams();
  params.append("id", id);
  try {
    const r = await fetch("/api/drivers/delete", {
      method: "POST",
      headers: { "Content-Type": "application/x-www-form-urlencoded" },
      body: params.toString(),
    });
    const data = await r.json();
    if (data.success) {
      notify("Водитель удалён");
      loadDrivers();
      loadCounters();
    } else notify(data.message || "Ошибка", "error");
  } catch (err) {
    notify("Сервер недоступен", "error");
  }
}

/* ===== Путевые листы ===== */
async function loadTrips() {
  const data = await fetchJSON("/api/trip_sheets");
  const container = document.getElementById("trips-list");
  if (!container) return;
  container.innerHTML = "";
  if (data.length === 0) {
    container.innerHTML =
      '<div style="text-align:center; color:#777; padding:20px;">Нет путевых листов</div>';
    return;
  }
  data.forEach((t) => {
    const div = document.createElement("div");
    div.className = "list-card";
    const status = t.closed
      ? '<span class="status-badge status-green">Закрыт</span>'
      : '<span class="status-badge status-yellow">Открыт</span>';
    div.innerHTML = `
            <div class="list-card-info">
                <div class="title">${t.number}</div>
                <div class="subtitle">${t.date} · ${t.route} · ${t.mileage_start} → ${t.mileage_end} км · ${t.fuel_consumption} л</div>
            </div>
            <div class="list-card-actions">${status}</div>
        `;
    container.appendChild(div);
  });
}
function openTripForm() {
  document.getElementById("trip-form").reset();
  document.getElementById("trip-modal").classList.remove("hidden");
}
function closeTripForm() {
  document.getElementById("trip-modal").classList.add("hidden");
}
async function onTripSubmit(e) {
  e.preventDefault();
  const fd = new FormData(e.target);
  const params = new URLSearchParams();
  for (const [k, v] of fd.entries()) params.append(k, v);
  try {
    const r = await fetch("/api/trip_sheets", {
      method: "POST",
      headers: { "Content-Type": "application/x-www-form-urlencoded" },
      body: params.toString(),
    });
    const data = await r.json();
    if (data.success) {
      closeTripForm();
      notify("Путевой лист добавлен");
      loadTrips();
    } else notify(data.message || "Ошибка", "error");
  } catch (err) {
    notify("Сервер недоступен", "error");
  }
}

/* ===== Полисы ===== */
async function loadPolicies() {
  const data = await fetchJSON("/api/policies");
  const container = document.getElementById("policies-list");
  if (!container) return;
  container.innerHTML = "";
  if (data.length === 0) {
    container.innerHTML =
      '<div style="text-align:center; color:#777; padding:20px;">Нет полисов</div>';
    return;
  }
  data.forEach((p) => {
    const div = document.createElement("div");
    div.className = "list-card";
    const type = p.type === "osago" ? "ОСАГО" : "КАСКО";
    const today = new Date();
    const until = new Date(p.valid_until);
    const diffDays = Math.ceil((until - today) / 86400000);
    let status = '<span class="status-badge status-green">Действует</span>';
    if (diffDays < 0)
      status = '<span class="status-badge status-red">Просрочен</span>';
    else if (diffDays <= 30)
      status = '<span class="status-badge status-yellow">Истекает</span>';
    div.innerHTML = `
            <div class="list-card-info">
                <div class="title">${type} ${p.number}</div>
                <div class="subtitle">${p.company} · до ${p.valid_until} · ${p.cost} ₽</div>
            </div>
            <div class="list-card-actions">${status}</div>
        `;
    container.appendChild(div);
  });
}
function openPolicyForm() {
  document.getElementById("policy-form").reset();
  document.getElementById("policy-modal").classList.remove("hidden");
}
function closePolicyForm() {
  document.getElementById("policy-modal").classList.add("hidden");
}
async function onPolicySubmit(e) {
  e.preventDefault();
  const fd = new FormData(e.target);
  const params = new URLSearchParams();
  for (const [k, v] of fd.entries()) params.append(k, v);
  try {
    const r = await fetch("/api/policies", {
      method: "POST",
      headers: { "Content-Type": "application/x-www-form-urlencoded" },
      body: params.toString(),
    });
    const data = await r.json();
    if (data.success) {
      closePolicyForm();
      notify("Полис добавлен");
      loadPolicies();
      loadCounters();
    } else notify(data.message || "Ошибка", "error");
  } catch (err) {
    notify("Сервер недоступен", "error");
  }
}

/* ===== ТО и техосмотры ===== */
async function loadMaintenance() {
  const data = await fetchJSON("/api/maintenance");
  const container = document.getElementById("maintenance-list");
  if (!container) return;
  container.innerHTML = "";
  if (data.length === 0) {
    container.innerHTML =
      '<div style="text-align:center; color:#777; padding:20px;">Нет записей о ТО</div>';
    return;
  }
  data.forEach((m) => {
    const div = document.createElement("div");
    div.className = "list-card";
    div.innerHTML = `
            <div class="list-card-info">
                <div class="title">${m.kind} (ТС #${m.vehicle_id})</div>
                <div class="subtitle">${m.date} · пробег ${m.mileage} км · результат: ${m.result} · следующее: ${m.next_date}</div>
            </div>
            <div class="list-card-actions">
                <button class="btn btn-delete" onclick="deleteMaintenance(${m.id})">Удалить</button>
            </div>
        `;
    container.appendChild(div);
  });
}
function openMaintenanceForm() {
  document.getElementById("maintenance-form").reset();
  document.getElementById("maintenance-modal").classList.remove("hidden");
}
function closeMaintenanceForm() {
  document.getElementById("maintenance-modal").classList.add("hidden");
}
async function onMaintenanceSubmit(e) {
  e.preventDefault();
  const fd = new FormData(e.target);
  const params = new URLSearchParams();
  for (const [k, v] of fd.entries()) params.append(k, v);
  try {
    const r = await fetch("/api/maintenance", {
      method: "POST",
      headers: { "Content-Type": "application/x-www-form-urlencoded" },
      body: params.toString(),
    });
    const data = await r.json();
    if (data.success) {
      closeMaintenanceForm();
      notify("Запись о ТО добавлена");
      loadMaintenance();
    } else notify(data.message || "Ошибка", "error");
  } catch (err) {
    notify("Сервер недоступен", "error");
  }
}
async function deleteMaintenance(id) {
  if (!confirm("Удалить запись о ТО?")) return;
  const params = new URLSearchParams();
  params.append("id", id);
  try {
    const r = await fetch("/api/maintenance/delete", {
      method: "POST",
      headers: { "Content-Type": "application/x-www-form-urlencoded" },
      body: params.toString(),
    });
    const data = await r.json();
    if (data.success) {
      notify("Запись удалена");
      loadMaintenance();
    } else notify(data.message || "Ошибка", "error");
  } catch (err) {
    notify("Сервер недоступен", "error");
  }
}

/* ===== Утилиты ===== */
async function fetchJSON(url) {
  try {
    const r = await fetch(url);
    return await r.json();
  } catch (e) {
    return [];
  }
}

window.switchTab = switchTab;
window.logout = logout;
window.openVehicleForm = openVehicleForm;
window.closeVehicleForm = closeVehicleForm;
window.deleteVehicle = deleteVehicle;
window.openDriverForm = openDriverForm;
window.closeDriverForm = closeDriverForm;
window.deleteDriver = deleteDriver;
window.openTripForm = openTripForm;
window.closeTripForm = closeTripForm;
window.openPolicyForm = openPolicyForm;
window.closePolicyForm = closePolicyForm;
window.openMaintenanceForm = openMaintenanceForm;
window.closeMaintenanceForm = closeMaintenanceForm;
window.deleteMaintenance = deleteMaintenance;
