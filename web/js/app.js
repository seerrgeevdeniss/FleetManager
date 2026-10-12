document.addEventListener("DOMContentLoaded", () => {
  const login = sessionStorage.getItem("login") || "admin";
  const avatar = document.getElementById("user-avatar");
  if (avatar) avatar.textContent = login.charAt(0).toUpperCase();

  switchTab("dashboard");

  document.querySelectorAll(".nav-item").forEach((btn) => {
    btn.addEventListener("click", () => switchTab(btn.dataset.tab));
  });

  loadCounters();
  loadVehicles();
  loadDrivers();

  const vForm = document.getElementById("vehicle-form");
  if (vForm) vForm.addEventListener("submit", onVehicleSubmit);

  const dForm = document.getElementById("driver-form");
  if (dForm) dForm.addEventListener("submit", onDriverSubmit);
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

function openInfo(section) {
  const modal = document.getElementById("modal");
  const title = document.getElementById("modal-title");
  const body = document.getElementById("modal-body");
  const titles = {
    trips: "Путевые листы",
    maintenance: "ТО и техосмотры",
    policies: "Полисы",
  };
  title.textContent = titles[section] || "Информация";
  body.textContent = "Здесь будет отображаться подробная информация.";
  modal.classList.remove("hidden");
}
function closeModal() {
  document.getElementById("modal").classList.add("hidden");
}

/* ===== Форма добавления ТС ===== */
function openVehicleForm() {
  document.getElementById("vehicle-modal").classList.remove("hidden");
}
function closeVehicleForm() {
  document.getElementById("vehicle-modal").classList.add("hidden");
  document.getElementById("vehicle-form").reset();
}
async function onVehicleSubmit(e) {
  e.preventDefault();
  const fd = new FormData(e.target);
  const params = new URLSearchParams();
  for (const [k, v] of fd.entries()) params.append(k, v);

  try {
    const r = await fetch("/api/vehicles", {
      method: "POST",
      headers: { "Content-Type": "application/x-www-form-urlencoded" },
      body: params.toString(),
    });
    const data = await r.json();
    if (data.success) {
      closeVehicleForm();
      loadVehicles();
      loadCounters();
    } else {
      alert(data.message || "Ошибка добавления");
    }
  } catch (err) {
    alert("Сервер недоступен");
  }
}

/* ===== Удаление ТС ===== */
async function deleteVehicle(id, gos_number) {
  if (!confirm(`Удалить транспортное средство ${gos_number}?`)) return;

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
      loadVehicles();
      loadCounters();
    } else {
      alert(data.message || "Ошибка удаления");
    }
  } catch (err) {
    alert("Сервер недоступен");
  }
}

/* ===== Форма добавления водителя ===== */
function openDriverForm() {
  document.getElementById("driver-modal").classList.remove("hidden");
}
function closeDriverForm() {
  document.getElementById("driver-modal").classList.add("hidden");
  document.getElementById("driver-form").reset();
}
async function onDriverSubmit(e) {
  e.preventDefault();
  const fd = new FormData(e.target);
  const params = new URLSearchParams();
  for (const [k, v] of fd.entries()) params.append(k, v);

  try {
    const r = await fetch("/api/drivers", {
      method: "POST",
      headers: { "Content-Type": "application/x-www-form-urlencoded" },
      body: params.toString(),
    });
    const data = await r.json();
    if (data.success) {
      closeDriverForm();
      loadDrivers();
      loadCounters();
    } else {
      alert(data.message || "Ошибка добавления");
    }
  } catch (err) {
    alert("Сервер недоступен");
  }
}

/* ===== Загрузка данных ===== */
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
    div.className = "dash-card";
    div.style.marginBottom = "8px";
    div.style.padding = "14px 16px";
    div.style.display = "flex";
    div.style.justifyContent = "space-between";
    div.style.alignItems = "center";
    div.style.cursor = "default";
    div.innerHTML = `
            <div style="text-align:left;">
                <div class="title">${v.gos_number} — ${v.brand} ${v.model}</div>
                <div class="subtitle">${v.year} · ${v.type} · ${v.mileage} км</div>
            </div>
            <button class="btn btn-delete" onclick="deleteVehicle(${v.id}, '${v.gos_number}')">Удалить</button>
        `;
    container.appendChild(div);
  });
}

async function loadDrivers() {
  const data = await fetchJSON("/api/drivers");
  const grid = document.getElementById("drivers-grid");
  if (!grid) return;
  grid.innerHTML = "";
  data.forEach((d) => {
    ["fio", "license_number", "categories"].forEach((field) => {
      const cell = document.createElement("div");
      cell.className = "table-cell";
      cell.title = d[field] || "";
      grid.appendChild(cell);
    });
  });
}

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
window.openInfo = openInfo;
window.closeModal = closeModal;
window.openVehicleForm = openVehicleForm;
window.closeVehicleForm = closeVehicleForm;
window.deleteVehicle = deleteVehicle;
window.openDriverForm = openDriverForm;
window.closeDriverForm = closeDriverForm;
