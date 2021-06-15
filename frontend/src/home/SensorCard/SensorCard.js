import "./style.css";

export default function SensorCard({name, value, icon, color}) {
    return <div className="sensor_card" style={{color: color, boxShadow: "1px 1px 8px 3px " + color + "BF"}}>
        <div className="sensor_card_header">
            <span className="sensor_card_name">{name}</span> 
            <i className={"sensor_card_icon fas " + icon}/> 
        </div>
        <span className="sensor_card_value"> {value}</span>
    </div>
}