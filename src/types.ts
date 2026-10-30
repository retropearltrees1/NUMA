export interface Message {
  id: string;
  role: "user" | "numa";
  text: string;
  category?: "calc" | "knowledge" | "conversation" | "unknown";
  timestamp: number;
}
