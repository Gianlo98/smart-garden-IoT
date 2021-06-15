import {useState, useEffect} from 'react';
import Plant from "./plant/plant";
import SensorCard from "./SensorCard/SensorCard";
import "./style.css";

export default function Home() {
    const deviceIp = "192.168.1.200";
    const sensorEndpoint = "/sensor";
    const configEndpoint = "/config";
  

    const [sensor, setSensor] = useState([]);
    const [config, setConfig] = useState([]);
  
    useEffect(() => {
        fetchSensor();
        fetchConfig();

        setInterval(() => {
            fetchConfig();
        }, 2000);
        
        setInterval(() => {
            fetchSensor();
        }, 5000);
          

    }, []);

    function fetchConfig() {
        fetch("http://" + deviceIp + configEndpoint)
        .then(res => res.json())
        .then(
          (result) => {
            setConfig(result);
          }
        )
    }

    function fetchSensor() {
        fetch("http://" + deviceIp + sensorEndpoint)
        .then(res => res.json())
        .then(
            (result) => {
            setSensor(result);
            }
        )
    }


    function handlePlantClick() {
        
        fetch("http://" + deviceIp + configEndpoint, {
            method: "POST", 
            body: JSON.stringify({
                shouldReadSensors: config.shouldReadSensors === "0"
            })
        })
        .then(res => res.json())
        .then(
            (result) => {
              setConfig(result);
            }
        )
    }

    const plantBackgroundColor = config.shouldReadSensors === "0" ? "#FF9E9D" :  "#E2F4DE" ;

    return (
        <div className="home">
            {(sensor !== []) && <div className="sensor_container_data"> 
                <SensorCard name="Light" value= {sensor.light} icon="fas fa-sun" color={"#F1A93A"}/>
                <SensorCard name="Temperature" value= {sensor.temp} icon="fas fa-thermometer-half" color={"#EE6F46"}/>
                <SensorCard name="Humidity" value= {sensor.humidity} icon="fas fa-tint" color={"#105E68"}/>
                <SensorCard name="Moisture" value= {sensor.moisture} icon="fas fa-seedling" color={"#323872"}/>
            </div>}
            <div onClick={handlePlantClick}>
                <Plant backgroundColor={plantBackgroundColor}/>
            </div>
        </div>
    );

}